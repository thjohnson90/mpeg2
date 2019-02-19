#ifndef __PADSTRM_H__
#define __PADSTRM_H__

class PesHeaderParser
{
public:
    PesHeaderParser(BitBuffer& bb, StreamState& ss);
    int32_t ParsePesHdr(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

class PesPacketParser
{
public:
    PesPacketParser(BitBuffer& bb, StreamState& ss);
    int32_t ChoosePesPacketParser(void);
    
protected:
    int32_t ParsePesPacket(void);
    int32_t ParsePesPadding(void);
    int32_t ParsePesDataBytes(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
