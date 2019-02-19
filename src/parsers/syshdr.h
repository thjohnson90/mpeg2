#ifndef __SYSHDR_H__
#define __SYSHDR_H__

class SystemHdrParser
{
public:
    SystemHdrParser(BitBuffer& bb, StreamState& ss);
    int32_t ParseSystemHdr(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
