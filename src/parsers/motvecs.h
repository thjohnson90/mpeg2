#ifndef __MOTIONVECS_H__
#define __MOTIONVECS_H__

class MotionVecsParser
{
public:
    MotionVecsParser(BitBuffer& bb, StreamState& ss);
    ~MotionVecsParser();
    
    int32_t Initialize(void);
    int32_t Destroy(void);
    
    int32_t ParseMotionVecs(PictureData* picData, uint32_t s);
    int32_t ParseMotionVec(PictureData* picData, uint32_t r, uint32_t s);
#ifdef TEST
        int32_t RunGetMotionCode(PictureData* picData, uint32_t r, uint32_t s, uint32_t t)
	{return GetMotionCode(picData, r, s, t);}
#endif
	
private:
    MotionVecsParser* GetMotionVecsParser(void);
    int32_t GetMotionCode(PictureData* picData, uint32_t r, uint32_t s, uint32_t t);
    int32_t GetDmVector(PictureData* picData, uint32_t t);

    MotionVecsParser* _motionvecsParser;
    BitBuffer&        _bitBuffer;
    StreamState&      _streamState;
};

#endif
