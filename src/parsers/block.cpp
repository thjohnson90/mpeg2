#include <stdint.h>
#include <iostream>
#include <assert.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "block.h"

BlockParser::BlockParser(BitBuffer& bb, StreamState& ss) :
    _bitBuffer(bb),
    _streamState(ss)
{
}

int32_t BlockParser::Initialize(void)
{
    return 0;
}

int32_t BlockParser::Destroy(void)
{
    return 0;
}

int32_t BlockParser::ParseBlock(PictureData* picData, uint32_t blkcnt)
{
    int32_t  status       = 0;
    int32_t  cc           = 0;
    uint32_t n            = 0;
    uint32_t run          = 0;
    int32_t  signed_level = 0;
    bool     eob          = false;
    
    do {
	if (nullptr == picData || 11 < blkcnt) {
	    status = -1;
	    break;
	}
	
	if (3 < blkcnt) {
	    if (0 == (blkcnt & 1)) {
		cc = 1;
	    } else {
		cc = 2;
	    }
	}
	
	if (picData->blkData.pattern_code[blkcnt]) {
	    if (1 == picData->macroblkData.macroblock_intra) {
		int32_t dct_diff   = 0;
		int32_t half_range = 0;
		
		if (blkcnt < 4) {
		    status = GetDctSizeLuminance(picData);
		    assert(-1 != status);

		    if (0 != picData->blkData.dct_dc_size_luminance) {
			picData->blkData.dct_dc_differential_lum =
			    _bitBuffer.GetBits(picData->blkData.dct_dc_size_luminance);
			
			dct_diff = GetDctDiff(picData,
					      picData->blkData.dct_dc_size_luminance,
					      static_cast<int32_t>(picData->blkData.dct_dc_differential_lum));
		    } else {
			dct_diff = 0;
		    }
		} else {
		    status = GetDctSizeChrominance(picData);
		    assert(-1 != status);

		    if (0 != picData->blkData.dct_dc_size_chrominance) {
			picData->blkData.dct_dc_differential_chrom =
			    _bitBuffer.GetBits(picData->blkData.dct_dc_size_chrominance);
			
			dct_diff = GetDctDiff(picData,
					      picData->blkData.dct_dc_size_chrominance,
					      static_cast<int32_t>(picData->blkData.dct_dc_differential_chrom));
		    } else {
			dct_diff = 0;
		    }
		}
		
		picData->blkData.QFS[0]          = picData->blkData.dct_dc_pred[cc] + dct_diff;
		picData->blkData.dct_dc_pred[cc] = picData->blkData.QFS[0];
		
		int32_t max = (1 << (8 + picData->picCodingExt.intra_dc_prec)) - 1;
		assert(0 <= picData->blkData.QFS[0]);
		assert(max >= max < picData->blkData.QFS[0]);

		n = 1;
	    } else {
		// First DCT coefficient
		status = ParseDctCoeff(picData, run, signed_level, eob, true);
		assert(-1 != status);

		status = FillQfsArray(picData, run, signed_level, n);
		assert(-1 != status);
	    }

	    while (!eob) {
		// Subsequent DCT coefficients
		status = ParseDctCoeff(picData, run, signed_level, eob, false);
		assert(-1 != status);

		if (!eob) {
		    status = FillQfsArray(picData, run, signed_level, n);
		    assert(-1 != status);
		}
	    }

	    while (n < 64) {
		picData->blkData.QFS[n] = 0;
		n++;
	    }
	}
    } while (0);

#if 0
    for (int i = 0; i < 64; i++) {
	if (0 == i % 8) {
	    cout << endl;
	}
	cout << "QFS[" << dec << i << "] =" << picData->blkData.QFS[i] << ", ";
    }
    cout << endl;
#endif
    return status;
}

int32_t BlockParser::GetDctSizeLuminance(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t bits   = 0;

    do {
	bits = _bitBuffer.PeekBits(9);

	if (4 == (bits >> 6)) {
	    _bitBuffer.GetBits(3);
	    picData->blkData.dct_dc_size_luminance = 0;
	} else if (0 == ((bits >> 7) & 2)) {
	    bits = _bitBuffer.GetBits(2);
	    if (0 == (bits & 1)) {
		picData->blkData.dct_dc_size_luminance = 1;
	    } else {
		picData->blkData.dct_dc_size_luminance = 2;
	    }
	} else if (5 == (bits >> 6)) {
	    _bitBuffer.GetBits(3);
	    picData->blkData.dct_dc_size_luminance = 3;
	} else if (6 == (bits >> 6)) {
	    _bitBuffer.GetBits(3);
	    picData->blkData.dct_dc_size_luminance = 4;
	} else if (0xE == (bits >> 5)) {
	    _bitBuffer.GetBits(4);
	    picData->blkData.dct_dc_size_luminance = 5;
	} else if (0x1E == (bits >> 4)) {
	    _bitBuffer.GetBits(5);
	    picData->blkData.dct_dc_size_luminance = 6;
	} else if (0x3E == (bits >> 3)) {
	    _bitBuffer.GetBits(6);
	    picData->blkData.dct_dc_size_luminance = 7;
	} else if (0x7E == (bits >> 2)) {
	    _bitBuffer.GetBits(7);
	    picData->blkData.dct_dc_size_luminance = 8;
	} else if (0xFE == (bits >> 1)) {
	    _bitBuffer.GetBits(8);
	    picData->blkData.dct_dc_size_luminance = 9;
	} else if (0x1FE == (bits & 0x1FE)) {
	    bits = _bitBuffer.GetBits(9);
	    if (0 == (bits & 1)) {
		picData->blkData.dct_dc_size_luminance = 10;
	    } else {
		picData->blkData.dct_dc_size_luminance = 11;
	    }
	} else {
	    status = -1;
	    break;
	}
    } while (0);

    return status;
}

int32_t BlockParser::GetDctSizeChrominance(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t bits   = 0;
    
    do {
	bits = _bitBuffer.PeekBits(10);
	
	if (0 == ((bits >> 8) & 2)) {
	    bits = _bitBuffer.GetBits(2);
	    if (0 == bits) {
		picData->blkData.dct_dc_size_chrominance = 0;
	    } else if (1 == bits) {
		picData->blkData.dct_dc_size_chrominance = 1;
	    } else {
		status = -1;
		break;
	    }
	} else if (2 == (bits >> 8)) {
	    _bitBuffer.GetBits(2);
	    picData->blkData.dct_dc_size_chrominance = 2;
	} else if (6 == (bits >> 7)) {
	    _bitBuffer.GetBits(3);
	    picData->blkData.dct_dc_size_chrominance = 3;
	} else if (0xE == (bits >> 6)) {
	    _bitBuffer.GetBits(4);
	    picData->blkData.dct_dc_size_chrominance = 4;
	} else if (0x1E == (bits >> 5)) {
	    _bitBuffer.GetBits(5);
	    picData->blkData.dct_dc_size_chrominance = 5;
	} else if (0x3E == (bits >> 4)) {
	    _bitBuffer.GetBits(6);
	    picData->blkData.dct_dc_size_chrominance = 6;
	} else if (0x7E == (bits >> 3)) {
	    _bitBuffer.GetBits(7);
	    picData->blkData.dct_dc_size_chrominance = 7;
	} else if (0xFE == (bits >> 2)) {
	    _bitBuffer.GetBits(8);
	    picData->blkData.dct_dc_size_chrominance = 8;
	} else if (0x1FE == (bits >> 1)) {
	    _bitBuffer.GetBits(9);
	    picData->blkData.dct_dc_size_chrominance = 9;
	} else if (0x3FE == (bits & 0x3FE)) {
	    bits = _bitBuffer.GetBits(10);
	    if (0 == (bits & 1)) {
		picData->blkData.dct_dc_size_chrominance = 10;
	    } else {
		picData->blkData.dct_dc_size_chrominance = 11;
	    }
	} else {
	    status = -1;
	    break;
	}
    } while (0);

    return status;
}

int32_t BlockParser::ParseDctCoeff(PictureData* picData,
				   uint32_t& run,
				   int32_t& signed_level,
				   bool& eob,
				   bool first)
{
    int32_t  status       = 0;
    uint32_t bits         = 0;
    bool     esc          = false;
    
    do {
	if (nullptr == picData) {
	    status = -1;
	    break;
	}
	
	if ((1 == picData->macroblkData.macroblock_intra) &&
	    (1 == picData->picCodingExt.intra_vlc_format)) {
	    // use B.15
	    status = GetB15Coeff(run, signed_level, eob, esc);
	    if (-1 == status || (first && eob)) {
		status = -1;
		break;
	    }
	} else {
	    // use B.14
	    status = GetB14Coeff(run, signed_level, eob, esc, first);
	    if (-1 == status || (first && eob)) {
		status = -1;
		break;
	    }
	}
    } while (0);

    return status;
}

int32_t BlockParser::GetDctDiff(PictureData* picData, uint32_t dct_dc_size, int32_t dct_dc_differential)
{
    int32_t dct_diff = 0;

    do {
	if (nullptr == picData) {
	    break;
	}

	int32_t half_range = 1 << (dct_dc_size - 1);

	if (dct_dc_differential >= half_range) {
	    dct_diff = dct_dc_differential;
	} else {
	    dct_diff = (dct_dc_differential + 1) - (2 * half_range);
	}
    } while (0);

    return dct_diff;
}

int32_t BlockParser::GetB14Coeff(uint32_t& run, int32_t& signed_level, bool& eob, bool& esc, bool first)
{
    int32_t  status = 0;
    uint32_t bits   = 0;
    uint32_t level  = 0;
    uint32_t mask   = 0;

    run   = 0;
    level = 0;
    eob   = false;
    esc   = false;
    
    do {
	bits = _bitBuffer.PeekBits(17);

	if ((!first) && (2 == (bits >> 15))) {
	    _bitBuffer.GetBits(2);
	    eob = true;
	    break;
	}

	if (first) {
	    GET_COEFF(2, 15, 17, 0, 1);
	} else {
	    GET_COEFF(6, 14, 17, 0, 1);
	}
	GET_COEFF(  6, 13, 17,  1,  1);
	GET_COEFF(  8, 12, 17,  0,  2);
	GET_COEFF(0xA, 12, 17,  2,  1);
	GET_COEFF(0xA, 11, 17,  0,  3);
	GET_COEFF(0xE, 11, 17,  3,  1);
	GET_COEFF(0xC, 11, 17,  4,  1);
	GET_COEFF(0xC, 10, 17,  1,  2);
	GET_COEFF(0xE, 10, 17,  5,  1);
	GET_COEFF(0xA, 10, 17,  6,  1);
	GET_COEFF(  8, 10, 17,  7,  1);
	GET_COEFF(0xC,  9, 17,  0,  4);
	GET_COEFF(  8,  9, 17,  2,  2);
	GET_COEFF(0xE,  9, 17,  8,  1);
	GET_COEFF(0xA,  9, 17,  9,  1);

	if (1 == (bits >> 11)) {
	    _bitBuffer.GetBits(6);
	    status = DecodeEscCoeff(run, signed_level);
	    assert(-1 != status);
	    esc = true;
	    break;
	}

	GET_COEFF(0x4C, 8, 17,  0,  5);
	GET_COEFF(0x42, 8, 17,  0,  6);
	GET_COEFF(0x4A, 8, 17,  1,  3);
	GET_COEFF(0x48, 8, 17,  3,  2);
	GET_COEFF(0x4E, 8, 17, 10,  1);
	GET_COEFF(0x46, 8, 17, 11,  1);
	GET_COEFF(0x44, 8, 17, 12,  1);
	GET_COEFF(0x40, 8, 17, 13,  1);
	GET_COEFF(0x14, 6, 17,  0,  7);
	GET_COEFF(0x18, 6, 17,  1,  4);
	GET_COEFF(0x16, 6, 17,  2,  3);
	GET_COEFF(0x1E, 6, 17,  4,  2);
	GET_COEFF(0x12, 6, 17,  5,  2);
	GET_COEFF(0x1C, 6, 17, 14,  1);
	GET_COEFF(0x1A, 6, 17, 15,  1);
	GET_COEFF(0x10, 6, 17, 16,  1);
	GET_COEFF(0x3A, 4, 17,  0,  8);
	GET_COEFF(0x30, 4, 17,  0,  9);
	GET_COEFF(0x26, 4, 17,  0, 10);
	GET_COEFF(0x20, 4, 17,  0, 11);
	GET_COEFF(0x36, 4, 17,  1,  5);
	GET_COEFF(0x28, 4, 17,  2,  4);
	GET_COEFF(0x38, 4, 17,  3,  3);
	GET_COEFF(0x24, 4, 17,  4,  3);
	GET_COEFF(0x3C, 4, 17,  6,  2);
	GET_COEFF(0x2A, 4, 17,  7,  2);
	GET_COEFF(0x22, 4, 17,  8,  2);
	GET_COEFF(0x3E, 4, 17, 17,  1);
	GET_COEFF(0x34, 4, 17, 18,  1);
	GET_COEFF(0x32, 4, 17, 19,  1);
	GET_COEFF(0x2E, 4, 17, 20,  1);
	GET_COEFF(0x2C, 4, 17, 21,  1);
	GET_COEFF(0x34, 3, 17,  0, 12);
	GET_COEFF(0x32, 3, 17,  0, 13);
	GET_COEFF(0x30, 3, 17,  0, 14);
	GET_COEFF(0x2E, 3, 17,  0, 15);
	GET_COEFF(0x2C, 3, 17,  1,  6);
	GET_COEFF(0x2A, 3, 17,  1,  7);
	GET_COEFF(0x28, 3, 17,  2,  5);
	GET_COEFF(0x26, 3, 17,  3,  4);
	GET_COEFF(0x24, 3, 17,  5,  3);
	GET_COEFF(0x22, 3, 17,  9,  2);
	GET_COEFF(0x20, 3, 17, 10,  2);
	GET_COEFF(0x3E, 3, 17, 22,  1);
	GET_COEFF(0x3C, 3, 17, 23,  1);
	GET_COEFF(0x3A, 3, 17, 24,  1);
	GET_COEFF(0x38, 3, 17, 25,  1);
	GET_COEFF(0x36, 3, 17, 26,  1);
	GET_COEFF(0x3E, 2, 17,  0, 16);
	GET_COEFF(0x3C, 2, 17,  0, 17);
	GET_COEFF(0x3A, 2, 17,  0, 18);
	GET_COEFF(0x38, 2, 17,  0, 19);
	GET_COEFF(0x36, 2, 17,  0, 20);
	GET_COEFF(0x34, 2, 17,  0, 21);
	GET_COEFF(0x32, 2, 17,  0, 22);
	GET_COEFF(0x30, 2, 17,  0, 23);
	GET_COEFF(0x2E, 2, 17,  0, 24);
	GET_COEFF(0x2C, 2, 17,  0, 25);
	GET_COEFF(0x2A, 2, 17,  0, 26);
	GET_COEFF(0x28, 2, 17,  0, 27);
	GET_COEFF(0x26, 2, 17,  0, 28);
	GET_COEFF(0x24, 2, 17,  0, 29);
	GET_COEFF(0x22, 2, 17,  0, 30);
	GET_COEFF(0x20, 2, 17,  0, 31);
	GET_COEFF(0x30, 1, 17,  0, 32);
	GET_COEFF(0x2E, 1, 17,  0, 33);
	GET_COEFF(0x2C, 1, 17,  0, 34);
	GET_COEFF(0x2A, 1, 17,  0, 35);
	GET_COEFF(0x28, 1, 17,  0, 36);
	GET_COEFF(0x26, 1, 17,  0, 37);
	GET_COEFF(0x24, 1, 17,  0, 38);
	GET_COEFF(0x22, 1, 17,  0, 39);
	GET_COEFF(0x20, 1, 17,  0, 40);
	GET_COEFF(0x3E, 1, 17,  1,  8);
	GET_COEFF(0x3C, 1, 17,  1,  9);
	GET_COEFF(0x3A, 1, 17,  1, 10);
	GET_COEFF(0x38, 1, 17,  1, 11);
	GET_COEFF(0x36, 1, 17,  1, 12);
	GET_COEFF(0x34, 1, 17,  1, 13);
	GET_COEFF(0x32, 1, 17,  1, 14);
	GET_COEFF(0x26, 0, 17,  1, 15);
	GET_COEFF(0x24, 0, 17,  1, 16);
	GET_COEFF(0x22, 0, 17,  1, 17);
	GET_COEFF(0x20, 0, 17,  1, 18);
	GET_COEFF(0x28, 0, 17,  6,  3);
	GET_COEFF(0x34, 0, 17, 11,  2);
	GET_COEFF(0x32, 0, 17, 12,  2);
	GET_COEFF(0x30, 0, 17, 13,  2);
	GET_COEFF(0x2E, 0, 17, 14,  2);
	GET_COEFF(0x2C, 0, 17, 15,  2);
	GET_COEFF(0x2A, 0, 17, 16,  2);
	GET_COEFF(0x3E, 0, 17, 27,  1);
	GET_COEFF(0x3C, 0, 17, 28,  1);
	GET_COEFF(0x3A, 0, 17, 29,  1);
	GET_COEFF(0x38, 0, 17, 30,  1);
	GET_COEFF(0x36, 0, 17, 31,  1);

	status = -1;
	break;
    } while (0);

    return status;
}

int32_t BlockParser::GetB15Coeff(uint32_t& run, int32_t& signed_level, bool& eob, bool& esc)
{
    int32_t  status = 0;
    uint32_t bits   = 0;
    uint32_t level  = 0;
    uint32_t mask   = 0;

    run   = 0;
    level = 0;
    eob   = false;
    esc   = false;
    
    do {
	bits = _bitBuffer.PeekBits(17);

	if (6 == (bits >> 13)) {
	    _bitBuffer.GetBits(4);
	    eob = true;
	    break;
	}

	GET_COEFF(    4, 14, 17,  0,  1);
	GET_COEFF(    4, 13, 17,  1,  1);
	GET_COEFF(  0xC, 13, 17,  0,  2);
	GET_COEFF(  0xA, 11, 17,  2,  1);
	GET_COEFF(  0xE, 12, 17,  0,  3);
	GET_COEFF(  0xE, 11, 17,  3,  1);
	GET_COEFF(  0xC, 10, 17,  4,  1);
	GET_COEFF(  0xC, 11, 17,  1,  2);
	GET_COEFF(  0xE, 10, 17,  5,  1);
	GET_COEFF(  0xC,  9, 17,  6,  1);
	GET_COEFF(    8,  9, 17,  7,  1);
	GET_COEFF( 0x38, 11, 17,  0,  4);
	GET_COEFF(  0xE,  9, 17,  2,  2);
	GET_COEFF(  0xA,  9, 17,  8,  1);
	GET_COEFF( 0xF0,  9, 17,  9,  1);

	if (1 == (bits >> 11)) {
	    _bitBuffer.GetBits(6);
	    status = DecodeEscCoeff(run, signed_level);
	    assert(-1 != status);
	    esc = true;
	    break;
	}

	GET_COEFF( 0x3A, 11, 17,  0,  5);
	GET_COEFF(  0xA, 10, 17,  0,  6);
	GET_COEFF( 0xF2,  9, 17,  1,  3);
	GET_COEFF( 0x4C,  8, 17,  3,  2);
	GET_COEFF( 0xF4,  9, 17, 10,  1);
	GET_COEFF( 0x42,  8, 17, 11,  1);
	GET_COEFF( 0x4A,  8, 17, 12,  1);
	GET_COEFF( 0x48,  8, 17, 13,  1);
	GET_COEFF(    8, 10, 17,  0,  7);
	GET_COEFF( 0x4E,  8, 17,  1,  4);
	GET_COEFF(0x1F8,  8, 17,  2,  3);
	GET_COEFF(0x1FA,  8, 17,  4,  2);
	GET_COEFF(    8,  7, 17,  5,  2);
	GET_COEFF(  0xA,  7, 17, 14,  1);
	GET_COEFF(  0xE,  7, 17, 15,  1);
	GET_COEFF( 0x1A,  6, 17, 16,  1);
	GET_COEFF( 0xF6,  9, 17,  0,  8);
	GET_COEFF( 0xF8,  9, 17,  0,  9);
	GET_COEFF( 0x46,  8, 17,  0, 10);
	GET_COEFF( 0x44,  8, 17,  0, 11);
	GET_COEFF( 0x40,  8, 17,  1,  5);
	GET_COEFF( 0x18,  6, 17,  2,  4);
	GET_COEFF( 0x38,  4, 17,  3,  3);
	GET_COEFF( 0x24,  4, 17,  4,  3);
	GET_COEFF( 0x3C,  4, 17,  6,  2);
	GET_COEFF( 0x2A,  4, 17,  7,  2);
	GET_COEFF( 0x22,  4, 17,  8,  2);
	GET_COEFF( 0x3E,  4, 17, 17,  1);
	GET_COEFF( 0x34,  4, 17, 18,  1);
	GET_COEFF( 0x32,  4, 17, 19,  1);
	GET_COEFF( 0x2E,  4, 17, 20,  1);
	GET_COEFF( 0x2C,  4, 17, 21,  1);
	GET_COEFF(0x1F4,  8, 17,  0, 12);
	GET_COEFF(0x1F6,  8, 17,  0, 13);
	GET_COEFF(0x1FC,  8, 17,  0, 14);
	GET_COEFF(0x1FE,  8, 17,  0, 15);
	GET_COEFF( 0x2C,  3, 17,  1,  6);
	GET_COEFF( 0x2A,  3, 17,  1,  7);
	GET_COEFF( 0x28,  3, 17,  2,  5);
	GET_COEFF( 0x26,  3, 17,  3,  4);
	GET_COEFF( 0x24,  3, 17,  5,  3);
	GET_COEFF( 0x22,  3, 17,  9,  2);
	GET_COEFF( 0x20,  3, 17, 10,  2);
	GET_COEFF( 0x3E,  3, 17, 22,  1);
	GET_COEFF( 0x3C,  3, 17, 23,  1);
	GET_COEFF( 0x3A,  3, 17, 24,  1);
	GET_COEFF( 0x38,  3, 17, 25,  1);
	GET_COEFF( 0x36,  3, 17, 26,  1);
	GET_COEFF( 0x3E,  2, 17,  0, 16);
	GET_COEFF( 0x3C,  2, 17,  0, 17);
	GET_COEFF( 0x3A,  2, 17,  0, 18);
	GET_COEFF( 0x38,  2, 17,  0, 19);
	GET_COEFF( 0x36,  2, 17,  0, 20);
	GET_COEFF( 0x34,  2, 17,  0, 21);
	GET_COEFF( 0x32,  2, 17,  0, 22);
	GET_COEFF( 0x30,  2, 17,  0, 23);
	GET_COEFF( 0x2E,  2, 17,  0, 24);
	GET_COEFF( 0x2C,  2, 17,  0, 25);
	GET_COEFF( 0x2A,  2, 17,  0, 26);
	GET_COEFF( 0x28,  2, 17,  0, 27);
	GET_COEFF( 0x26,  2, 17,  0, 28);
	GET_COEFF( 0x24,  2, 17,  0, 29);
	GET_COEFF( 0x22,  2, 17,  0, 30);
	GET_COEFF( 0x20,  2, 17,  0, 31);
	GET_COEFF( 0x30,  1, 17,  0, 32);
	GET_COEFF( 0x2E,  1, 17,  0, 33);
	GET_COEFF( 0x2C,  1, 17,  0, 34);
	GET_COEFF( 0x2A,  1, 17,  0, 35);
	GET_COEFF( 0x28,  1, 17,  0, 36);
	GET_COEFF( 0x26,  1, 17,  0, 37);
	GET_COEFF( 0x24,  1, 17,  0, 38);
	GET_COEFF( 0x22,  1, 17,  0, 39);
	GET_COEFF( 0x20,  1, 17,  0, 40);
	GET_COEFF( 0x3E,  1, 17,  1,  8);
	GET_COEFF( 0x3C,  1, 17,  1,  9);
	GET_COEFF( 0x3A,  1, 17,  1, 10);
	GET_COEFF( 0x38,  1, 17,  1, 11);
	GET_COEFF( 0x36,  1, 17,  1, 12);
	GET_COEFF( 0x34,  1, 17,  1, 13);
	GET_COEFF( 0x32,  1, 17,  1, 14);
	GET_COEFF( 0x26,  0, 17,  1, 15);
	GET_COEFF( 0x24,  0, 17,  1, 16);
	GET_COEFF( 0x22,  0, 17,  1, 17);
	GET_COEFF( 0x20,  0, 17,  1, 18);
	GET_COEFF( 0x28,  0, 17,  6,  3);
	GET_COEFF( 0x34,  0, 17, 11,  2);
	GET_COEFF( 0x32,  0, 17, 12,  2);
	GET_COEFF( 0x30,  0, 17, 13,  2);
	GET_COEFF( 0x2E,  0, 17, 14,  2);
	GET_COEFF( 0x2C,  0, 17, 15,  2);
	GET_COEFF( 0x2A,  0, 17, 16,  2);
	GET_COEFF( 0x3E,  0, 17, 27,  1);
	GET_COEFF( 0x3C,  0, 17, 28,  1);
	GET_COEFF( 0x3A,  0, 17, 29,  1);
	GET_COEFF( 0x38,  0, 17, 30,  1);
	GET_COEFF( 0x36,  0, 17, 31,  1);

	status = -1;
	break;
    } while (0);

    return status;
}

int32_t BlockParser::DecodeEscCoeff(uint32_t run, int32_t signed_level)
{
    int32_t  status = 0;
    uint32_t level  = 0;

    do {
	run   = _bitBuffer.GetBits(6);
	level = _bitBuffer.GetBits(12);

	if (0 == level || 0x800 == level) {
	    status = -1;
	    break;
	}

	signed_level =
	    0x800 == (level & 0x800) ? static_cast<int32_t>(-level) : static_cast<int32_t>(level);
    } while (0);

    return status;
}

int32_t BlockParser::FillQfsArray(PictureData* picData, uint32_t run, int32_t signed_level, uint32_t& n)
{
    int32_t status = 0;

    do {
	if (64 < n + run) {
	    status = -1;
	    break;
	}

	for (int m = 0; m < run; m++) {
	    picData->blkData.QFS[n] = 0;
	    n++;
	}
	
	picData->blkData.QFS[n] = signed_level;
	n++;
    } while (0);

    return status;
}
