#ifndef __BITBUF_H__
#define __BITBUF_H__

class BitBuffer
{
public:
    BitBuffer();
    
    uint8_t  GetNextStartCode(void);
    uint32_t GetBitCount(void) {return _bitBufCnt;}
    uint8_t  GetLastStartCode(void) {return _lastStartCode;}
    bool     PeekStartCodePrefix(void);
    uint8_t  PeekNextStartCode(void);
    
    virtual uint32_t GetByte(void)                        = 0;
    virtual uint32_t GetBytes(uint8_t* buf, uint32_t len) = 0;
    virtual uint32_t GetBits(uint32_t bitCnt)             = 0;
    virtual uint32_t PeekBits(uint32_t bitCnt)            = 0;
    virtual uint32_t FillBitBuffer(void)                  = 0;
    
    static const uint32_t BITS_IN_BYTE;
    static const uint32_t MAX_BITS_IN_BUF;
    
    enum _parseSet {START0, START1, START2, START_CMD} parseState;
    
protected:
    uint64_t  _bitBuf;
    uint32_t  _bitBufCnt;
    uint8_t   _lastStartCode;
};

#endif
