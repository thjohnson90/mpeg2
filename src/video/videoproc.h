#ifndef __VIDEOPROC_H__
#define __VIDEOPROC_H__

class VideoProcessor
{
public:
    VideoProcessor();
    ~VideoProcessor();
    
    static VideoProcessor* GetInstance(void);
    int32_t Initialize(StreamState* ss);
    int32_t Destroy(void);
    int32_t ProcessVideoBlock(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t ProcessSkippedMacroblock(PictureData* picData);
    int32_t GetAlternateScan(int32_t (&src)[64], int32_t (&dst)[8][8], uint32_t alt);
    int32_t DoMotionCompensation(PictureData* picData);

    int32_t Ring(uint32_t id, uint32_t cmd) {_cmd[id] = cmd; _bell[id].Ring();}

private:
    static void* VidProcWorker(void* arg);
    
    int32_t DoInverseQuantization(StreamState* sState, PictureData* picData, uint32_t blkcnt);
    int32_t DoSaturationAndMismatch(PictureData* picData, uint32_t blkcnt);
    int32_t DoInverseDCT(PictureData* picData, uint32_t blkcnt);
    int32_t ScheduleInverseDCTWork(PictureData* picData, uint32_t blkcnt);

    Thread                 _worker[NUM_HW_THREADS];
    Doorbell               _bell[NUM_HW_THREADS];
    uint32_t               _cmd[NUM_HW_THREADS];
    StreamState*           _streamState;
    static VideoProcessor* _instance;

    struct _ThrdArgs {
	VideoProcessor* pThis;
	uint32_t        id;
    } _thrdArgs[NUM_HW_THREADS];
};

#endif
