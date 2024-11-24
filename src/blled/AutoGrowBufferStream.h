//Code from Xtouchautogrowstream
#ifndef AutoGrowBufferStream_h
#define AutoGrowBufferStream_h

#include <Arduino.h>
#include <Stream.h>

#define BUFFER_INCREMENTS 128

class AutoGrowBufferStream : public Stream
{
private:
    uint16_t _len;
    uint16_t buffer_size;
    char* _buffer;

public:
    AutoGrowBufferStream() {
        this->_len = 0;
        this->_buffer = (char*)malloc(BUFFER_INCREMENTS);
        this->buffer_size = BUFFER_INCREMENTS;
    }

    ~AutoGrowBufferStream() {
        free(this->_buffer);
    }

    virtual size_t write(uint8_t byte) {
        if (this->_len + 1 > this->buffer_size) {
            auto tmp = (char*)realloc(this->_buffer, this->buffer_size + BUFFER_INCREMENTS);
            if (tmp == NULL) {
                Serial.println(F("Failed to grow buffer"));
                return 0;
            }
            this->_buffer = tmp;
            this->buffer_size += BUFFER_INCREMENTS;
        }
        this->_buffer[this->_len] = byte;
        this->_len++;
        return 1;
    }

    virtual int read() {
        return 0;
    }

    virtual int available() {
        return 1;
    }

    virtual void flush() {
        this->_len = 0;
        this->_buffer = (char*)realloc(this->_buffer, BUFFER_INCREMENTS);
        this->buffer_size = BUFFER_INCREMENTS;
    }

    int peek() {
        return 0;
    }

    const uint16_t current_length() const { return _len; }
    const char* get_buffer() const { return _buffer; }
    const char* get_string() const { 
        _buffer[_len] = '\0';
        return _buffer;
    }

    using Print::write;
};

#endif
