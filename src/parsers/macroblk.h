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
    MacroblkParser*   GetMacroblkParser(void);
    MotionVecsParser* GetMotionVecsParser(void);

    uint32_t        GetMacroblkAddrInc(void);
    int32_t         GetMacroblkModes(PictureData* picData);
    int32_t         MbModeIPic(PictureData* picData);
    int32_t         MbModePPic(PictureData* picData);
    int32_t         MbModeBPic(PictureData* picData);

    MacroblkParser*   _macroblkParser;
    MotionVecsParser* _motionvecsParser;
    BitBuffer&        _bitBuffer;
    StreamState&      _streamState;
};

#endif
