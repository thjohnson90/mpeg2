#ifndef __VIDEOPROC_H__
#define __VIDEOPROC_H__

class VideoProcessor
{
public:
    VideoProcessor();
    ~VideoProcessor();
    
    static VideoProcessor* GetInstance(void);
    int32_t Initialize(void);
    int32_t Destroy(void);
    int32_t ProcessVideoBlock(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t GetAlternateScan(int32_t (&src)[64], int32_t (&dst)[8][8], uint32_t alt);

    int32_t Ring(uint32_t cmd) {_cmd = cmd; _bell.Ring();}

private:
    static void* VidProcWorker(void* arg);
    
    int32_t DoInverseQuantization(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t DoSaturationAndMismatch(PictureData* picData);
    int32_t DoInverseDCT(PictureData* picData);

    Thread   _worker;
    Doorbell _bell;
    uint32_t _cmd;
    static VideoProcessor* _instance;
};

#endif
