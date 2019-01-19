#ifndef __FILE_BITBUF_H__
#define __FILE_BITBUF_H__

class FileBitBuffer : public BitBuffer
{
public:
    FileBitBuffer(ifstream& in);
    
    virtual uint32_t GetByte(void);
    virtual uint32_t GetBytes(uint8_t* buf, uint32_t len);
    virtual uint32_t GetBits(uint32_t bitCnt);
    virtual uint32_t PeekBits(uint32_t bitCnt);
    virtual uint32_t FillBitBuffer(void);
    
private:
    ifstream& _inFile;
};

#endif
