#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "pack.h"

PackHdrParser::PackHdrParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t PackHdrParser::ParsePackHdr(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    
    do
    {
        // parse the pack header
	marker                               = _bitBuffer.GetBits(2);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.packHdr.SCR32_30        = _bitBuffer.GetBits(3);
	marker                               = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.packHdr.SCR29_15        = _bitBuffer.GetBits(15);
	marker   = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.packHdr.SCR14_00        = _bitBuffer.GetBits(15);
	marker   = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.packHdr.SCR_EXT         = _bitBuffer.GetBits(9);
	marker   = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.packHdr.ProgramMuxRate  = _bitBuffer.GetBits(22);
	marker   = _bitBuffer.GetBits(2);
	CHECK_VALUE_AND_BREAK(3, marker, status, -1);
	_streamState.packHdr.Reserved        = _bitBuffer.GetBits(5);
	_streamState.packHdr.PackStuffingLen = _bitBuffer.GetBits(3);

        // read stuffing bytes
        if (0 != _streamState.packHdr.PackStuffingLen) {
            uint8_t* pBuf = new uint8_t[_streamState.packHdr.PackStuffingLen];

            status = _bitBuffer.GetBytes(pBuf, _streamState.packHdr.PackStuffingLen);
	    delete [] pBuf;
        }
    } while (0);
    
    return status;
}
