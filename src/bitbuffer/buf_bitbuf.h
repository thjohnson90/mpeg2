#ifndef __BUF_BITBUF_H__
#define __BUF_BITBUF_H__

class BufBitBuffer : public BitBuffer
{
public:
    BufBitBuffer();
    ~BufBitBuffer();

    virtual uint32_t GetByte(void);
    virtual int32_t  GetBytes(uint8_t* buf, uint32_t len);
    virtual uint32_t GetBits(uint32_t bitCnt);
    virtual uint32_t PeekBits(uint32_t bitCnt);
    virtual int32_t  FillBitBuffer(void);
    
    uint8_t* GetEmptyBuffer(uint32_t sz);
    int32_t  SetBaseParser(BaseParser* bp);
#ifdef TEST
    void     PokeBits(uint64_t bits, uint32_t len) {
	_bitBuf = (bits << (MAX_BITS_IN_BUF - len));
	_bitBufCnt = len;
    }
#endif
    
private:
    uint8_t*    _rawBuf;
    uint32_t    _size;
    uint32_t    _offset;
    BaseParser* _bparser;
};

#endif
