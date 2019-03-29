#include <stdint.h>
#include <iostream>
#include <assert.h>
#include <math.h>

#include "stream.h"
#include "picdata.h"
#include "doorbell.h"
#include "thread.h"
#include "videoproc.h"

using namespace std;

static const double C0   = 1.0 / sqrt(static_cast<double>(2.0));
static const double Cn   = 1.0;
static const double PI   = 3.14159265359;
static const double N    = 8.0;
static const double Nx2  = 16.0;
static const double HALF = 0.5;

static double V   = 0.0;
static double U   = 0.0;
static double Y   = 0.0;
static double X   = 0.0;
static double tmp = 0.0;
    
#ifdef TEST
uint32_t inv_scan[2][8][8] {
#else
const static uint32_t inv_scan[2][8][8] {
#endif
    {
	{ 0,  1,  5,  6, 14, 15, 27, 28},
	{ 2,  4,  7, 13, 16, 26, 29, 42},
	{ 3,  8, 12, 17, 25, 30, 41, 43},
	{ 9, 11, 18, 24, 31, 40, 44, 53},
	{10, 19, 23, 32, 39, 45, 52, 54},
	{20, 22, 33, 38, 46, 51, 55, 60},
	{21, 34, 37, 47, 50, 56, 59, 61},
	{35, 36, 48, 49, 57, 58, 62, 63}
    },
    {
	{ 0,  4,  6, 20, 22, 36, 38, 52},
	{ 1,  5,  7, 21, 23, 37, 39, 53},
	{ 2,  8, 19, 24, 34, 40, 50, 54},
	{ 3,  9, 18, 25, 35, 41, 51, 55},
	{10, 17, 26, 30, 42, 46, 56, 60},
	{11, 16, 27, 31, 43, 47, 57, 61},
	{12, 15, 28, 32, 44, 48, 58, 62},
	{13, 14, 29, 33, 45, 49, 59, 63}
    }
};

#ifdef TEST
int32_t quan_scale[2][32] {
#else
const static int32_t quan_scale[2][32] {
#endif
    {0, 2, 4, 6, 8, 10, 12, 14,
	    16, 18, 20, 22, 24, 26, 28, 30,
	    32, 34, 36, 38, 40, 42, 44, 46,
	    48, 50, 52, 54, 56, 58, 60, 62},
    {0, 1, 2, 3, 4, 5, 6, 7,
	    8,  10, 12, 14, 16, 18, 20, 22,
	    24, 28, 32, 36, 40, 44, 48, 52,
	    56, 64, 72, 80, 88, 96, 104, 112}
};

// need to declare these two arrays as constant
extern const int32_t dflt_intra_quant_matrix[8][8] {
    { 8, 16, 19, 22, 26, 27, 29, 34},
    {16, 16, 22, 24, 27, 29, 34, 37},
    {19, 22, 26, 27, 29, 34, 34, 38},
    {22, 22, 26, 27, 29, 34, 37, 40},
    {22, 26, 27, 29, 32, 35, 40, 48},
    {26, 27, 29, 32, 35, 40, 48, 58},
    {26, 27, 29, 34, 38, 46, 56, 69},
    {27, 29, 35, 38, 46, 56, 69, 83}
};

extern const int32_t dflt_non_intra_quant_matrix[8][8] {
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16}
};

VideoProcessor* VideoProcessor::_instance = nullptr;

VideoProcessor::VideoProcessor() : _cmd(0)
{
}

VideoProcessor::~VideoProcessor()
{
}

VideoProcessor* VideoProcessor::GetInstance(void)
{
    if (nullptr == _instance) {
	_instance = new VideoProcessor();
    }

    return _instance;
}
				     
int32_t VideoProcessor::Initialize(void)
{
    _worker.Initialize(VidProcWorker, this);
    
    return 0;
}

int32_t VideoProcessor::Destroy(void)
{
    cout << "Ringing worker..." << endl;
    _worker.Ring(Thread::parse_cmd_data_ready);
    _bell.Listen();
    cout << "Received " << _cmd << " from worker..." << endl;
    cout << "Ringing worker..." << endl;
    _worker.Ring(Thread::parse_cmd_exit);
    _bell.Listen();
    cout << "Received " << _cmd << " from worker..." << endl;
    cout << "Joining worker..." << endl;
    _worker.Join(nullptr);
    
    return 0;
}

void* VideoProcessor::VidProcWorker(void* arg)
{
    uint32_t cmd = 0;
    
    VideoProcessor& vidProc = *(static_cast<VideoProcessor*>(arg));
    
    do {
	cout << "Worker is listening..." << endl;
	vidProc._worker.Listen();
	cmd = vidProc._worker.GetCmd();
	cout << "Worker received msg: " << cmd << endl;
	vidProc.Ring(Thread::parse_cmd_data_consumed);
    } while (Thread::parse_cmd_exit != cmd);
    
    cout << "Worker is exiting..." << endl;

    return arg;
}

int32_t VideoProcessor::ProcessVideoBlock(StreamState* sState, PictureData* picData, uint32_t blkcnt)
{
    int32_t status = 0;

    do {
	status =
	    GetAlternateScan(picData->blkData.QFS, picData->blkData.QF, picData->picCodingExt.alternate_scan);
	assert(-1 != status);

	status = DoInverseQuantization(sState, picData, blkcnt);
	assert(-1 != status);

	status = DoSaturationAndMismatch(picData);
	assert(-1 != status);

	status = DoInverseDCT(picData);
	assert(-1 != status);
    } while (0);

    return status;
}

int32_t VideoProcessor::GetAlternateScan(int32_t (&src)[64], int32_t (&dst)[8][8], uint32_t alt)
{
    int32_t status = 0;

    do {
	int32_t v   = 0;
	int32_t u   = 0;
	int32_t alt = 0;

	for (v = 0; v < 8; v++) {
	    for (u = 0; u < 8; u++) {
		dst[v][u] = src[inv_scan[alt][v][u]];
	    }
	}
    } while (0);

    return status;
}

int32_t VideoProcessor::DoInverseQuantization(StreamState* sState, PictureData* picData, uint32_t blkcnt)
{
    int32_t status          = 0;
    int32_t v               = 0;
    int32_t u               = 0;
    int32_t w               = 0;
    int32_t k               = 0;
    int32_t cc              = 0;
    int32_t intra_dc_mult   = 0;
    int32_t quantiser_scale = 0;

    switch (picData->picCodingExt.intra_dc_prec) {
    case 0:
	intra_dc_mult = 8;
	break;
    case 1:
	intra_dc_mult = 4;
	break;
    case 2:
	intra_dc_mult = 2;
	break;
    case 3:
	intra_dc_mult = 1;
	break;
    default:
	cout << "Illegal intra_dc_precision" << endl;
	assert(1 != 1);
	break;
    }
    
    cc = GetCC(blkcnt);
    assert(0 != picData->sliceData.quantiser_scale_code);
    quantiser_scale =
	quan_scale[picData->picCodingExt.q_scale_type][picData->sliceData.quantiser_scale_code];
    
    if (1 == picData->macroblkData.macroblock_intra) {
	if ((sequence_extension::CHROMA_FMT_420 != sState->extData.seqExt.chroma_format) &&
	    (0 != cc)) {
	    w = 2;
	}
    } else {
	w = 1;
	k = 0 > picData->blkData.QF[v][u] ? -1 : 0 == picData->blkData.QF[v][u] ? 0 : 1;
	if ((sequence_extension::CHROMA_FMT_420 != sState->extData.seqExt.chroma_format) &&
	    (0 != cc)) {
	    w = 3;
	}
    }
    
    do {
	for (v = 0; v < 8; v++) {
	    for (u = 0; u < 8; u++) {
		if ((0 == u) && (0 == v) && (1 == picData->macroblkData.macroblock_intra)) {
		    // dc coefficient of intra macroblock
		    picData->blkData.Fpp[v][u] = intra_dc_mult * picData->blkData.QF[v][u];
		} else {
		    // all other coefficients
		    picData->blkData.Fpp[v][u] =
			((2 * picData->blkData.QF[v][u] + k) * sState->seqHdr.W[w][v][u] * quantiser_scale) / 32;
		}
	    }
	}
    } while (0);
    
    return status;
}

int32_t VideoProcessor::DoSaturationAndMismatch(PictureData* picData)
{
    int32_t status = 0;
    int32_t v      = 0;
    int32_t u      = 0;
    int32_t sum    = 0;

    do {
	for (v = 0; v < 8; v++) {
	    for (u = 0; u < 8; u++) {
		picData->blkData.Fp[v][u] = picData->blkData.Fpp[v][u] > 2047 ? 2047 :
		    picData->blkData.Fpp[v][u] < -2048 ? -2048 : picData->blkData.Fpp[v][u];

		sum += picData->blkData.Fp[v][u];
		picData->blkData.F[v][u] = picData->blkData.Fp[v][u];
	    }
	}

	if (0 == (sum & 1)) {
	    if (0 != (picData->blkData.F[7][7] & 1)) {
		picData->blkData.F[7][7] = picData->blkData.Fp[7][7] - 1;
	    } else {
		picData->blkData.F[7][7] = picData->blkData.Fp[7][7] + 1;
	    }
	}
    } while (0);

    return status;
}

int32_t VideoProcessor::DoInverseDCT(PictureData* picData)
{
    int32_t status = 0;
    int32_t x      = 0;
    int32_t y      = 0;
    int32_t v      = 0;
    int32_t u      = 0;

#if 0
    do {
	for (y = 0; y < 8; y++) {
	    for (x = 0; x < 8; x++) {
		tmp = 0.0;
		Y   = static_cast<double>(y);
		X   = static_cast<double>(x);
		for (v = 0; v < 8; v++) {
		    for (u = 0; u < 8; u++) {
			V = static_cast<double>(v);
			U = static_cast<double>(u);
			
			if ((0 == v) && (0 == u)) {
			    tmp += (HALF * static_cast<double>(picData->blkData.F[v][u]) *
				    cos(((2.0 * Y + 1.0) * V * PI) / Nx2) *
				    cos(((2.0 * X + 1.0) * U * PI) / Nx2));
			} else {
			    tmp += (static_cast<double>(picData->blkData.F[v][u]) *
				    cos(((2.0 * Y + 1.0) * V * PI) / Nx2) *
				    cos(((2.0 * X + 1.0) * U * PI) / Nx2));
			}
		    }
		}
		picData->blkData.f[y][x] = tmp * (2.0 / N);
	    }
	}
    } while (0);
#endif	
    return status;
}

