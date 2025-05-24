#ifndef LOGSERIAL_H
#define LOGSERIAL_H

#include <Arduino.h>
#include <MycilaWebSerial.h>

class AsyncWebServer;

class LogSerialClass : public Stream {
private:
WebSerial webSerial;
public:
  void begin(unsigned long baud = 115200) {
    Serial.begin(baud);
  }

  void begin(AsyncWebServer* server, unsigned long baud = 115200, size_t bufferSize = 100) {
    Serial.begin(baud);
    webSerial.begin(server);
    webSerial.setBuffer(bufferSize);
    webSerial.onMessage([](const std::string& msg) {
      Serial.print("[WebSerial RX] ");
      Serial.println(msg.c_str());
    });
  }

  void onMessage(std::function<void(const std::string&)> cb) {
    webSerial.onMessage(cb);
  }

  void setBuffer(size_t size) {
    webSerial.setBuffer(size);
  }

  int available() override { return Serial.available(); }
  int read() override { return Serial.read(); }
  int peek() override { return Serial.peek(); }
  void flush() override { Serial.flush(); }

  size_t write(uint8_t b) override {
    Serial.write(b);
    webSerial.write(&b, 1);
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    Serial.write(buffer, size);
    webSerial.write(buffer, size);
    return size;
  }

  using Print::write;
  operator bool() { return (bool)Serial; }


};

LogSerialClass LogSerial;

#endif // LOGSERIAL_H
