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
    int32_t CodedBlkPattern(PictureData* picData);

#ifdef TEST
    uint32_t RunGetMacroblkAddrInc(void) {return GetMacroblkAddrInc();}
    uint32_t RunGetMacroblkModes(PictureData* picData) {return GetMacroblkModes(picData);}
#endif
    
private:
    MacroblkParser*   GetMacroblkParser(void);
    MotionVecsParser* GetMotionVecsParser(void);
    BlockParser*      GetBlockParser(void);
    
    uint32_t        GetMacroblkAddrInc(void);
    int32_t         GetMacroblkModes(PictureData* picData);
    int32_t         MbModeIPic(PictureData* picData);
    int32_t         MbModePPic(PictureData* picData);
    int32_t         MbModeBPic(PictureData* picData);
    int32_t         GetPatternCode(PictureData* picData);
    
    MacroblkParser*   _macroblkParser;
    MotionVecsParser* _motionvecsParser;
    BlockParser*      _blockParser;
    BitBuffer&        _bitBuffer;
    StreamState&      _streamState;
};

#endif
