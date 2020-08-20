#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _capacity(capacity), _bytes_written(0), _bytes_read(0), _input_ended(false), _buffer(deque<uint8_t>()) {}

size_t ByteStream::write(const string &data) {
    if (_input_ended) {
        _error = true;
        return 0;
    }
    size_t write_len = data.length() <= remaining_capacity() ? data.length() : remaining_capacity();
    copy(data.begin(), data.begin() + write_len, back_inserter(_buffer));
    _bytes_written += write_len;

    return write_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if (_buffer.size() <= 0) {
        return "";
    }
    size_t peek_len = len <= _buffer.size() ? len : _buffer.size();
    string s(_buffer.begin(), _buffer.begin() + peek_len);
    return s;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (_buffer.size() <= 0) {
        return;
    }
    size_t pop_len = len <= _buffer.size() ? len : _buffer.size();
    for (size_t i = 0; i < pop_len; i++)
        _buffer.pop_front();
    _bytes_read += pop_len;
}

void ByteStream::end_input() { _input_ended = true; }

bool ByteStream::input_ended() const { return _input_ended; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return input_ended() && _buffer.empty(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
