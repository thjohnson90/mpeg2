#ifndef __SLICE_H__
#define __SLICE_H__

class SliceParser
{
public:
    SliceParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParseSliceData(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
