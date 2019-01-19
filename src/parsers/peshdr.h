#ifndef __PADSTRM_H__
#define __PADSTRM_H__

class PesHeaderParser
{
public:
    PesHeaderParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParsePesHdr(void);
    
protected:
    uint32_t ParsePesPacket(void);
    uint32_t ParsePesPadding(void);
    uint32_t ParsePesDataBytes(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
