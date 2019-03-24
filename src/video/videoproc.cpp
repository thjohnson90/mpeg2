#include <stdint.h>
#include <iostream>
#include <assert.h>

#include "stream.h"
#include "picdata.h"
#include "videoproc.h"

using namespace std;

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

VideoProcessor::VideoProcessor(StreamState& ss) :
    _streamState(ss)
{
}

int32_t VideoProcessor::ProcessVideoBlock(PictureData* picData, uint32_t blkcnt)
{
    int32_t status = 0;

    do {
	status = GetAlternateScan(picData);
	if (-1 == status) {
	    break;
	}

	status = DoInverseQuantization(picData, blkcnt);
	if (-1 == status) {
	    break;
	}
    } while (0);

    return status;
}


int32_t VideoProcessor::GetAlternateScan(PictureData* picData)
{
    int32_t status = 0;

    do {
	int32_t v   = 0;
	int32_t u   = 0;
	int32_t alt = 0;

	alt = picData->picCodingExt.alternate_scan;
	
	for (v = 0; v < 8; v++) {
	    for (u = 0; u < 8; u++) {
		picData->blkData.QF[v][u] = picData->blkData.QFS[inv_scan[alt][v][u]];
	    }
	}
    } while (0);

    return status;
}

int32_t VideoProcessor::DoInverseQuantization(PictureData* picData, uint32_t blkcnt)
{
    int32_t status        = 0;
    int32_t v             = 0;
    int32_t u             = 0;
    int32_t w             = 0;
    int32_t cc            = 0;
    int32_t intra_dc_mult = 0;

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
    if (1 == picData->macroblkData.macroblock_intra) {
	if ((sequence_extension::CHROMA_FMT_420 != _streamState.extData.seqExt.chroma_format) &&
	    (0 != cc)) {
	    w = 2;
	}
    } else {
	w = 1;
	if ((sequence_extension::CHROMA_FMT_420 != _streamState.extData.seqExt.chroma_format) &&
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
		}
	    }
	}
    } while (0);
    
    return status;
}
