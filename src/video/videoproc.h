#ifndef __VIDEOPROC_H__
#define __VIDEOPROC_H__

class VideoProcessor
{
public:
    VideoProcessor(StreamState& ss);
    int32_t ProcessVideoBlock(PictureData* picData, uint32_t blkcnt);

private:
    int32_t GetAlternateScan(PictureData* picData);
    int32_t DoInverseQuantization(PictureData* picData, uint32_t blkcnt);

    StreamState& _streamState;
};

#endif
