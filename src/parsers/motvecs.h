#ifndef __MOTIONVECS_H__
#define __MOTIONVECS_H__

class MotionVecsParser
{
public:
    MotionVecsParser(BitBuffer& bb, StreamState& ss);
    ~MotionVecsParser();
    
    int32_t Initialize(void);
    int32_t Destroy(void);
    
    int32_t ParseMotionVecs(uint32_t s);
    int32_t ParseMotionVec(uint32_t r, uint32_t s);
    
private:
    MotionVecsParser* GetMotionVecsParser(void);

    MotionVecsParser* _motionvecsParser;
    BitBuffer&        _bitBuffer;
    StreamState&      _streamState;
};

#endif
