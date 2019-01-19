#ifndef __GOP_H__
#define __GOP_H__

class GopHdrParser
{
public:
    GopHdrParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParseGopHdr(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
