#ifndef __BLOCK_H__
#define __BLOCK_H__

class BlockParser
{
public:
    BlockParser(BitBuffer& bb, StreamState& ss);

    int32_t Initialize(void);
    int32_t Destroy(void);

    int32_t CodedBlkPattern(PictureData* picData);
    int32_t ParseBlock(PictureData* picData, uint32_t blkcnt);

private:
    int32_t GetPatternCode(PictureData* picData);
    
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
