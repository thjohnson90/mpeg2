#ifndef __PACK_H__
#define __PACK_H__

class PackHdrParser
{
public:
    PackHdrParser(BitBuffer& bb, StreamState& ss);
    int32_t ParsePackHdr(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
