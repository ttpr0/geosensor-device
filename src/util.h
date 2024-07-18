#pragma once

#include <RTClib.h>

namespace util
{

// utiliy span class to give vector-like interface to underlying buffer
template <typename T>
class span
{
private:
    T* buffer;
    size_t len;
    size_t cap;

public:
    span(T* buffer, size_t cap) : buffer(buffer), len(0), cap(cap) {}
    T* begin() { return buffer; }
    T* end() { return buffer + len; }
    T& operator[](size_t i) const { return buffer[i]; }
    T* data() const { return buffer; }
    size_t size() const { return len; }
    size_t capacity() const { return cap; }
    void clear() { len = 0; }
    void push_back(const T& t)
    {
        if (this->len >= this->cap) {
            return;
        }
        buffer[len++] = t;
    }
};

// binary buffer
// used to encode binary messages
class buffer
{
private:
    byte* _data;
    size_t _len;
    size_t _cap;

public:
    buffer(byte* buffer, size_t cap) : _data(buffer), _len(0), _cap(cap) {}

    byte* data() const { return _data; }

    size_t size() const { return _len; }

    size_t capacity() const { return _cap; }

    void clear() { _len = 0; }

    // writes 1 byte
    void write_char(char c)
    {
        const byte* t = (const byte*)&c;
        _data[_len++] = t[0];
    }

    // writes 4 bytes
    void write_float(float f)
    {
        const byte* t = (const byte*)&f;
        _data[_len++] = t[0];
        _data[_len++] = t[1];
        _data[_len++] = t[2];
        _data[_len++] = t[3];
    }

    // writes 7 bytes [yymdhms]
    void write_time(const DateTime& time)
    {
        auto year = time.year();
        const byte* t = (const byte*)&year;
        _data[_len++] = t[0];
        _data[_len++] = t[1];
        _data[_len++] = time.month();
        _data[_len++] = time.day();
        _data[_len++] = time.hour();
        _data[_len++] = time.minute();
        _data[_len++] = time.second();
    }
};
}  // namespace util