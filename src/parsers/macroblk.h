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
    uint32_t        GetMacroblkAddrInc(void);
    int32_t         GetMacroblkMode(PictureData* picData);
    int32_t         MbModeIPic(PictureData* picData);
    int32_t         MbModePPic(PictureData* picData);
    int32_t         MbModeBPic(PictureData* picData);

    MacroblkParser* _macroblkParser;
    BitBuffer&      _bitBuffer;
    StreamState&    _streamState;
};

#endif
