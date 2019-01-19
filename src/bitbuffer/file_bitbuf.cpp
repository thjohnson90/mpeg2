#include <fstream>
#include <stdint.h>

using namespace std;

#include "bitbuf.h"
#include "file_bitbuf.h"

FileBitBuffer::FileBitBuffer(ifstream& in) : BitBuffer(), _inFile(in)
{
}

uint32_t FileBitBuffer::GetByte(void)
{
    uint32_t bits_to_next_byte;
    
    bits_to_next_byte = _bitBufCnt % BITS_IN_BYTE;
    if (0 != bits_to_next_byte) {
        GetBits(bits_to_next_byte);
    }
    
    return GetBits(BITS_IN_BYTE);
}

uint32_t FileBitBuffer::GetBytes(uint8_t* buf, uint32_t len)
{
    uint32_t status = 0;
    uint32_t index  = 0;
    
    do {
        if (nullptr == buf || 0 == len) {
            status = -1;
            break;
        }
        
        while (0 != GetBitCount())
        {
            buf[index] = GetByte();
            len--;
            index++;
        }
        
        _inFile.get(reinterpret_cast<char*>(&buf[index]), len);
    } while (0);
    
    return status;
}

uint32_t FileBitBuffer::GetBits(uint32_t bitCnt)
{
    uint64_t tmp = 0ULL;
    do {
        if (_bitBufCnt < bitCnt) {
            FillBitBuffer();
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

uint32_t FileBitBuffer::PeekBits(uint32_t bitCnt)
{
    uint64_t bits = 0;
    
    do {
        if (bitCnt > _bitBufCnt)
        {
            FillBitBuffer();
        }
        
        uint64_t mask = (1 << bitCnt) - 1;
        bits = _bitBuf;
        bits >>= (MAX_BITS_IN_BUF - bitCnt);
        bits &= mask;
    } while (0);
    
    return static_cast<uint32_t>(bits);
}

uint32_t FileBitBuffer::FillBitBuffer()
{
    uint32_t status    = 0;
    uint32_t newBitCnt = 0;
    uint64_t tmpBuf    = 0ULL;
    
    while ((_bitBufCnt + newBitCnt + BITS_IN_BYTE) <= BitBuffer::MAX_BITS_IN_BUF)
    {
        tmpBuf <<= BITS_IN_BYTE;
        tmpBuf |= static_cast<uint64_t>(_inFile.get());
        // TODO: throw if read failed
        newBitCnt += BITS_IN_BYTE;
    }
    
    _bitBuf |= (tmpBuf << (MAX_BITS_IN_BUF - _bitBufCnt - newBitCnt));
    _bitBufCnt += newBitCnt;
    
    return status;
}
