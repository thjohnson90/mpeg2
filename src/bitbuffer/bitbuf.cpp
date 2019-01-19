#include <iostream>
#include <stdint.h>

using namespace std;

#include "bitbuf.h"

const uint32_t BitBuffer::BITS_IN_BYTE    = 8;
const uint32_t BitBuffer::MAX_BITS_IN_BUF = 64;

BitBuffer::BitBuffer() : _bitBuf(0), _bitBufCnt(0)
{
}

uint8_t BitBuffer::GetNextStartCode(void)
{
    uint8_t startCode = 0;
    bool startCodeFound     = false;
    parseState              = START0;
    
    do {
        startCode = GetByte();
        switch (startCode) {
        case 0:
            if (START0 == parseState) {
                parseState = START1;
            } else if (START1 == parseState) {
                parseState = START2;
            } else if (START2 == parseState) {
                // got another zero in the start code prefix - just maintain state
                parseState = START2;
            } else if (START_CMD == parseState) {
                // picture start code
                cout << "Start Code: 0x" << hex << uppercase << (static_cast<unsigned int>(startCode) & 0xFF) << endl;
                startCodeFound = true;
                _lastStartCode = startCode;
                parseState = START0;
            } else {
                parseState = START0;
            }
            break;
            
        case 1:
            if (START2 == parseState) {
                parseState = START_CMD;
            } else if (START_CMD == parseState) {
                // slice start code
                cout << "Start Code: 0x" << hex << uppercase << (static_cast<unsigned int>(startCode) & 0xFF) << endl;
                startCodeFound = true;
                _lastStartCode = startCode;
                parseState = START0;
            } else {
                parseState = START0;
            }
            break;
            
        default:
            if (START_CMD != parseState) {
                startCode = 0;
            }
            else {
                cout << "Start Code: 0x" << hex << uppercase << (static_cast<unsigned int>(startCode) & 0xFF) << endl;
                startCodeFound = true;
                _lastStartCode = startCode;
            }
            parseState = START0;
            break;
        }
    } while (!startCodeFound);
    
    return startCode;
}

bool BitBuffer::PeekStartCodePrefix(void)
{
    bool startPrefixFound = false;
    
    FillBitBuffer();
    
    uint64_t tmp = _bitBuf;
    tmp >>= 40;
    if (1 == tmp) {
        startPrefixFound = true;
    }
    
    return startPrefixFound;
}

uint8_t BitBuffer::PeekNextStartCode(void)
{
    uint8_t sc = 0;
    
    if (PeekStartCodePrefix()) {
	uint64_t tmp = _bitBuf;

	sc = (tmp >>= 32);
	sc &= 0xFF;
    }

    return sc;
}
