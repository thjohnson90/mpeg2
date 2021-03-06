#ifndef __SLICE_H__
#define __SLICE_H__

class SliceParser
{
public:
    SliceParser(BitBuffer& bb, StreamState& ss);
    ~SliceParser();
    
    int32_t Initialize(void);
    int32_t Destroy(void);
    
    int32_t ParseSliceData(void);
    
private:
    MacroblkParser* GetMacroblkParser(void);
    
    MacroblkParser* _macroblkParser;
    BitBuffer&      _bitBuffer;
    StreamState&    _streamState;
};

#endif
