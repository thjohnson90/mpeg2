#ifndef __PICTURE_H__
#define __PICTURE_H__

class PictureParser
{
public:
    PictureParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParsePictureHdr(void);
    uint32_t ParsePictCodingExt(void);
    uint32_t ParsePictData(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
