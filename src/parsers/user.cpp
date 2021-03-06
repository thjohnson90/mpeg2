#include <stdint.h>
#include <iostream>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "user.h"

UserDataParser::UserDataParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t UserDataParser::ParseUserData(void)
{
    int32_t status = 0;
    uint8_t byte   = 0;

    do {
	while (!_bitBuffer.PeekStartCodePrefix()) {
	    byte = _bitBuffer.GetByte();
	}

	if (0 <= status) {
	    _bitBuffer.GetNextStartCode();
	}
    } while (0);
    
    return status;
}


