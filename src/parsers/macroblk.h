#ifndef __MACROBLK_H__
#define __MACROBLK_H__

class MacroblkParser
{
public:
    MacroblkParser(BitBuffer& bb, StreamState& ss);
    ~MacroblkParser();
    
    int32_t Initialize(void);
    int32_t Destroy(void);
    
    int32_t ParseMacroblkData(void);
    
private:
    MacroblkParser* GetMacroblkParser(void);
    
    MacroblkParser* _macroblkParser;
    BitBuffer&      _bitBuffer;
    StreamState&    _streamState;
};

#endif
