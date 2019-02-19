#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "gop.h"

GopHdrParser::GopHdrParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t GopHdrParser::ParseGopHdr(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    
    do {
        _streamState.gop.drop_frame_flag    = _bitBuffer.GetBits(1);
        _streamState.gop.time_code_hours    = _bitBuffer.GetBits(5);
        _streamState.gop.time_code_minutes  = _bitBuffer.GetBits(6);
        marker                              = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.gop.time_code_seconds  = _bitBuffer.GetBits(6);
        _streamState.gop.time_code_pictures = _bitBuffer.GetBits(6);
        _streamState.gop.closed_gop         = _bitBuffer.GetBits(1);
        _streamState.gop.broken_link        = _bitBuffer.GetBits(1);

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}
