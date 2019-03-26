#ifndef __VIDEOPROC_H__
#define __VIDEOPROC_H__

class VideoProcessor
{
public:
    VideoProcessor();
    ~VideoProcessor();
    
    static VideoProcessor* GetInstance(void);
    int32_t ProcessVideoBlock(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t GetAlternateScan(int32_t (&src)[64], int32_t (&dst)[8][8], uint32_t alt);

private:
    int32_t DoInverseQuantization(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t DoSaturation(PictureData* picData);

    static VideoProcessor* _instance;
};

#endif
