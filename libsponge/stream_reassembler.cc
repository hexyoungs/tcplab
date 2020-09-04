#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _chunks(), _output(capacity), _capacity(capacity), _pos(0), _max_pos(0), _eof(false) {
    make_heap(_chunks.begin(), _chunks.end(), DataChunkCmp());
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (_eof)
        return;
    if (_max_pos < index + data.length())
        _max_pos = index + data.length();

    DataChunk chunk = DataChunk{data, index, eof};
    _chunks.push_back(chunk);
    push_heap(_chunks.begin(), _chunks.end(), DataChunkCmp());

    while (!_chunks.empty() && !_eof) {
        DataChunk c = _chunks.front();
        if (c.index < _pos) {
            // duplicated packet
            // or overlapped packet
            int extra = c.data.length() + c.index - _pos;
            if (extra > 0) {
                // more data to write
                _pos += _output.write(c.data.substr(c.data.length() - extra));
            }
        } else if (c.index == _pos) {
            _pos += _output.write(c.data);
        } else {
            // should wait more
            break;
        }

        pop_heap(_chunks.begin(), _chunks.end(), DataChunkCmp());
        _chunks.pop_back();
        if (c.eof) {
            _eof = true;
            _output.end_input();
        }
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t unassembled = 0;
    for (auto &item : _chunks)
        unassembled += item.data.length();
    size_t unassembled_by_pos = _pos == 0 ? (_max_pos > 1 ? (_max_pos - 1) : 0) : (_max_pos - _pos);

    return std::min(unassembled, unassembled_by_pos);
}

bool StreamReassembler::empty() const { return _pos == 0; }
