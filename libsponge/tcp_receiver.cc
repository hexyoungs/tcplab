#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

#include <iostream>

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    if (seg.header().syn && !_isn.has_value()) {
        _offset = seg.header().fin ? 2 : 1;
        _isn = optional<WrappingInt32>{seg.header().seqno};
        _reassembler.push_substring(string(seg.payload().str()),
                                    unwrap(seg.header().seqno, _isn.value(), stream_out().bytes_written()),
                                    seg.header().fin);
        return true;
    } else if (_isn.has_value()) {
        // check overlap
        cout << "seg.header().seqno - ackno().value(): " << seg.header().seqno - ackno().value() << endl;
        int overlap = seg.header().seqno - ackno().value() + int(seg.payload().size());
        if (overlap >= 0 && (seg.header().seqno - ackno().value() < 0 ||
                             size_t(seg.header().seqno - ackno().value()) < window_size())) {
            if (overlap == 0 && !seg.header().fin) {
                return false;
            }
            string s = string(seg.payload().str());
            uint64_t index = unwrap(seg.header().seqno - _offset, _isn.value(), stream_out().bytes_written());
            cout << "s: " << s << endl;
            cout << "index: " << index << endl;
            _reassembler.push_substring(s, index, seg.header().fin);
            if (seg.header().fin) {
                _offset = 2;
            }
            return true;
        }
        return false;
    }
    return false;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_isn.has_value())
        return nullopt;

    return optional<WrappingInt32>(wrap(stream_out().bytes_written() + _offset, _isn.value()));
}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }
