#ifndef _BBLPRINTERDISCOVERY_H
#define _BBLPRINTERDISCOVERY_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include "filesystem.h"  // for saveFileSystem()

#define BBL_SSDP_PORT 2021
#define BBL_SSDP_MCAST_IP IPAddress(239, 255, 255, 250)
#define BBL_DISCOVERY_INTERVAL 10000UL
#define BBL_MAX_PRINTERS 10

struct BBLPrinter {
  IPAddress ip;
  char usn[64];
};

static WiFiUDP bblUdp;
static bool bblUdpInitialized = false;
static unsigned long bblLastDiscovery = 0;

static BBLPrinter bblLastKnownPrinters[BBL_MAX_PRINTERS];
static int bblKnownPrinterCount = 0;

bool bblIsPrinterKnown(IPAddress ip, int* index = nullptr) {
  for (int i = 0; i < bblKnownPrinterCount; i++) {
    if (bblLastKnownPrinters[i].ip == ip) {
      if (index) *index = i;
      return true;
    }
  }
  return false;
}

void bblPrintKnownPrinters() {
  LogSerial.println("[BBLScan] Known printers:");
  if (bblKnownPrinterCount == 0) {
    LogSerial.println("  (none)");
  }
  for (int i = 0; i < bblKnownPrinterCount; i++) {
    LogSerial.printf("  [%d] IP: %s", i + 1, bblLastKnownPrinters[i].ip.toString().c_str());
    if (strlen(bblLastKnownPrinters[i].usn) > 0) {
      LogSerial.printf("  [USN: %s]", bblLastKnownPrinters[i].usn);
    }
    LogSerial.println();
  }
}

void bblSearchPrinters() {
  unsigned long now = millis();
  if (now - bblLastDiscovery < BBL_DISCOVERY_INTERVAL) return;
  bblLastDiscovery = now;

  if (!bblUdpInitialized) {
    bblUdp.beginMulticast(BBL_SSDP_MCAST_IP, BBL_SSDP_PORT);
    bblUdpInitialized = true;
  }

  String msearch =
    "M-SEARCH * HTTP/1.1\r\n"
    "HOST: 239.255.255.250:2021\r\n"
    "MAN: \"ssdp:discover\"\r\n"
    "MX: 5\r\n"
    "ST: urn:bambulab-com:device:3dprinter:1\r\n\r\n";

  for (int i = 0; i < 2; i++) {
    bblUdp.beginPacket(BBL_SSDP_MCAST_IP, BBL_SSDP_PORT);
    bblUdp.print(msearch);
    bblUdp.endPacket();
    delay(250);
  }

  if (printerConfig.debuging) {
    LogSerial.println("[BBLScan] Searching for printers...");
  }

  unsigned long start = millis();
  int sessionFound = 0;
  IPAddress seenIPs[BBL_MAX_PRINTERS];
  int seenCount = 0;

  while (millis() - start < 5000) {
    int size = bblUdp.parsePacket();
    if (size) {
      IPAddress senderIP = bblUdp.remoteIP();

      bool alreadySeen = false;
      for (int i = 0; i < seenCount; i++) {
        if (seenIPs[i] == senderIP) {
          alreadySeen = true;
          break;
        }
      }
      if (alreadySeen) continue;
      if (seenCount < BBL_MAX_PRINTERS) seenIPs[seenCount++] = senderIP;

      char buffer[512];
      int len = bblUdp.read(buffer, sizeof(buffer) - 1);
      buffer[len] = 0;

      String response(buffer);
      String usnStr = "";
      int usnPos = response.indexOf("USN:");
      if (usnPos >= 0) {
        int end = response.indexOf("\r\n", usnPos);
        usnStr = response.substring(usnPos + 4, end);
        usnStr.trim();
      }

      // IP update check for stored USN
      if (usnStr.length() > 0 && strcmp(printerConfig.serialNumber, usnStr.c_str()) == 0) {
        String currentIP = senderIP.toString();
        if (String(printerConfig.printerIP) != currentIP) {
          LogSerial.printf("[BBLScan] Detected matching USN with updated IP (%s → %s). Saving...\n", printerConfig.printerIP, currentIP.c_str());
          strlcpy(printerConfig.printerIP, currentIP.c_str(), sizeof(printerConfig.printerIP));
          saveFileSystem();
        }
      }

      int existingIndex = -1;
      bool isNewPrinter = !bblIsPrinterKnown(senderIP, &existingIndex);

      if (printerConfig.debuging || (printerConfig.debugingchange && isNewPrinter)) {
        LogSerial.printf("[BBLScan]  [%d] IP: %s", ++sessionFound, senderIP.toString().c_str());
        if (usnStr.length()) {
          LogSerial.printf("  [USN: %s]", usnStr.c_str());
        }
        LogSerial.println();
      }

      if (isNewPrinter && bblKnownPrinterCount < BBL_MAX_PRINTERS) {
        BBLPrinter& printer = bblLastKnownPrinters[bblKnownPrinterCount++];
        printer.ip = senderIP;
        strncpy(printer.usn, usnStr.c_str(), sizeof(printer.usn) - 1);
        printer.usn[sizeof(printer.usn) - 1] = 0;
      }
    }
    delay(10);
  }

  if (printerConfig.debuging && sessionFound == 0) {
    LogSerial.println("[BBLScan] No printers found.");
  }

  if (printerConfig.debuging) {
    bblPrintKnownPrinters();
  }
}

#endif
