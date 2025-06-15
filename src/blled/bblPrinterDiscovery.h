#ifndef _BBLPRINTERDISCOVERY_H
#define _BBLPRINTERDISCOVERY_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <lwip/etharp.h>  // for ARP lookup

#define BBL_SSDP_PORT 2021
#define BBL_SSDP_MCAST_IP IPAddress(239, 255, 255, 250)
#define BBL_DISCOVERY_INTERVAL 10000UL
#define BBL_MAX_PRINTERS 10

struct BBLPrinter {
  IPAddress ip;
  uint8_t mac[6];
  char usn[64];
};

static WiFiUDP bblUdp;
static bool bblUdpInitialized = false;
static unsigned long bblLastDiscovery = 0;

static BBLPrinter bblLastKnownPrinters[BBL_MAX_PRINTERS];
static int bblKnownPrinterCount = 0;

bool bblIsPrinterKnown(IPAddress ip) {
  for (int i = 0; i < bblKnownPrinterCount; i++) {
    if (bblLastKnownPrinters[i].ip == ip) return true;
  }
  return false;
}

void bblPrintKnownPrinters() {
  LogSerial.println("[BBLScan] Known printers:");
  if (bblKnownPrinterCount == 0) {
    LogSerial.println("  (none)");
  }
  for (int i = 0; i < bblKnownPrinterCount; i++) {
    LogSerial.printf("  [%d] IP: %s  MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                  i + 1,
                  bblLastKnownPrinters[i].ip.toString().c_str(),
                  bblLastKnownPrinters[i].mac[0], bblLastKnownPrinters[i].mac[1],
                  bblLastKnownPrinters[i].mac[2], bblLastKnownPrinters[i].mac[3],
                  bblLastKnownPrinters[i].mac[4], bblLastKnownPrinters[i].mac[5]);
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

  // Send twice
  for (int i = 0; i < 2; i++) {
    bblUdp.beginPacket(BBL_SSDP_MCAST_IP, BBL_SSDP_PORT);
    bblUdp.print(msearch);
    bblUdp.endPacket();
    delay(250);
  }

  LogSerial.println("[BBLScan] Searching for printers...");

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

      eth_addr* mac;
      ip4_addr_t ip4;
      ip4.addr = static_cast<uint32_t>(senderIP);
      const ip4_addr_t* dummyPtr;
      bool macFound = (etharp_find_addr(netif_list, &ip4, &mac, &dummyPtr) == ERR_OK);

      String response(buffer);
      String usnStr = "";
      int usnPos = response.indexOf("USN:");
      if (usnPos >= 0) {
        int end = response.indexOf("\r\n", usnPos);
        usnStr = response.substring(usnPos + 4, end);
        usnStr.trim();
      }

      LogSerial.printf("  [%d] IP: %s", ++sessionFound, senderIP.toString().c_str());
      if (macFound) {
        LogSerial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                      mac->addr[0], mac->addr[1], mac->addr[2],
                      mac->addr[3], mac->addr[4], mac->addr[5]);
      } else {
        LogSerial.print("  MAC: (not found)");
      }
      if (usnStr.length()) {
        LogSerial.printf("  [USN: %s]", usnStr.c_str());
      }
      LogSerial.println();

      if (!bblIsPrinterKnown(senderIP) && bblKnownPrinterCount < BBL_MAX_PRINTERS) {
        BBLPrinter& printer = bblLastKnownPrinters[bblKnownPrinterCount++];
        printer.ip = senderIP;
        if (macFound) {
          memcpy(printer.mac, mac->addr, 6);
        } else {
          memset(printer.mac, 0, 6);
        }
        strncpy(printer.usn, usnStr.c_str(), sizeof(printer.usn) - 1);
        printer.usn[sizeof(printer.usn) - 1] = 0;
      }
    }
    delay(10);
  }

  if (sessionFound == 0) {
    LogSerial.println("[BBLScan] No printers found.");
  }

  bblPrintKnownPrinters();
}

#endif
