#include <iostream>
#include <fstream>
#include <stdint.h>

#include "stream.h"
#include "bitbuf.h"
#include "pack.h"
#include "syshdr.h"
#include "peshdr.h"
#include "sequence.h"
#include "extension.h"
#include "user.h"
#include "gop.h"
#include "picdata.h"
#include "motvecs.h"
#include "doorbell.h"
#include "thread.h"
#include "thrdcmds.h"
#include "videoproc.h"
#include "block.h"
#include "macroblk.h"
#include "slice.h"
#include "picture.h"
#include "base_parser.h"
#include "thrdcmds.h"

using namespace std;

#include "bitbuf.h"
#include "buf_bitbuf.h"

BufBitBuffer::BufBitBuffer() :
    BitBuffer(), _rawBuf(0), _size(0), _offset(0), _bparser(nullptr)
{
}

BufBitBuffer::~BufBitBuffer()
{
    delete [] _rawBuf;
    _rawBuf = nullptr;
    _size   = 0;
    _offset = 0;
}

uint32_t BufBitBuffer::GetByte(void)
{
    uint32_t bits_to_next_byte;
    
    bits_to_next_byte = _bitBufCnt % BITS_IN_BYTE;
    if (0 != bits_to_next_byte) {
        GetBits(bits_to_next_byte);
    }
    
    return GetBits(BITS_IN_BYTE);
}

int32_t BufBitBuffer::GetBytes(uint8_t* buf, uint32_t len)
{
    uint32_t status = 0;
    uint32_t index  = 0;
    
    do {
        if (nullptr == buf || 0 == len) {
            status = -1;
            break;
        }

	// use up bits in the bit buffer first
        while (BITS_IN_BYTE <= _bitBufCnt && 0 != len)
        {
            buf[index] = GetByte();
            len--;
            index++;
        }

	if (len > (_size - _offset)) {
	    // not enough bytes left in buffer to satisfy request
	    status = -1;
	    break;
	}

	// if we need more bytes read directly from the buffer
	if (0 != len) {
	    do {
		buf[index] = _rawBuf[_offset];
		index++;
		_offset++;
		len--;
	    } while (0 != len);
	}
    } while (0);
    
    return status;
}

uint32_t BufBitBuffer::GetBits(uint32_t bitCnt)
{
    uint64_t tmp = 0ULL;
    
    do {
//	if (32 < bitCnt) {
	    // exceeded max bits available in return type
	    // TODO: consider using uint64_t types in StreamState
//	}
	
        if (_bitBufCnt < bitCnt) {
            if (-1 == FillBitBuffer()) {
		// reached eof and bit buffer is empty
		break;
	    }
        }
        
        uint64_t mask = (1 << bitCnt) - 1;
        tmp = _bitBuf;
        tmp >>= (MAX_BITS_IN_BUF - bitCnt);
        tmp &= mask;
        _bitBuf <<= bitCnt;
        _bitBufCnt -= bitCnt;
    } while (0);
    
    return static_cast<uint32_t>(tmp);
}

uint32_t BufBitBuffer::PeekBits(uint32_t bitCnt)
{
    uint64_t bits = 0;
    
    do {
        if (bitCnt > _bitBufCnt) {
            if (-1 == FillBitBuffer()) {
		// reached eof and bit buffer is empty
		break;
	    }
        }
        
        uint64_t mask = (1 << bitCnt) - 1;
        bits = _bitBuf;
        bits >>= (MAX_BITS_IN_BUF - bitCnt);
        bits &= mask;
    } while (0);
    
    return static_cast<uint32_t>(bits);
}

int32_t BufBitBuffer::FillBitBuffer()
{
    uint32_t status    = 0;
    uint32_t newBitCnt = 0;
    uint64_t tmpBuf    = 0ULL;
    
    do {
	if (0 == (_size - _offset)) {
	    // buffer is empty
	    _bparser->Ring(parse_cmd::data_consumed);
	    
	    // wait for more_data message
	    _bparser->WorkerListen();
	    if (common_cmd::exit == _bparser->GetWorkerCmd()) {
		status = -1;
		break;
	    }
	}
	
	while ((_bitBufCnt + newBitCnt + BITS_IN_BYTE) <= BitBuffer::MAX_BITS_IN_BUF)
	{
	    if (0 != (_size - _offset)) {
		// there is at least one more byte available in the file
		tmpBuf <<= BITS_IN_BYTE;
		tmpBuf |= static_cast<uint64_t>(_rawBuf[_offset]);
		newBitCnt += BITS_IN_BYTE;
		_offset++;
	    } else {
		// there are no bytes available in the buffer
		if (0 >= _bitBufCnt + newBitCnt) {
		    // no bytes available in the file and not bits in the bit buffer
		    status = -1;
		}
		break;
	    }
	}
	
	_bitBuf |= (tmpBuf << (MAX_BITS_IN_BUF - _bitBufCnt - newBitCnt));
	_bitBufCnt += newBitCnt;
    } while (0);
    
    return status;
}

uint8_t* BufBitBuffer::GetEmptyBuffer(uint32_t sz)
{
    uint8_t* pbuf = nullptr;
    
    do {
	if (nullptr != _rawBuf && sz != _size) {
	    delete [] _rawBuf;
	    _rawBuf = nullptr;
	    _size   = 0;
	    _offset = 0;
	}

	if (nullptr == _rawBuf) {
	    _rawBuf = new uint8_t[sz];
	    if (nullptr == _rawBuf) {
		break;
	    }
	}

	_size   = sz;
	_offset = 0;
	pbuf    = _rawBuf;
    } while (0);

    return pbuf;
}

int32_t BufBitBuffer::SetBaseParser(BaseParser* bp)
{
    int32_t status = 0;

    do {
	if (nullptr == bp) {
	    status = -1;
	    break;
	}
	_bparser = bp;
    } while (0);

    return status;
}
    
    
