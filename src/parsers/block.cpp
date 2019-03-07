#include <stdint.h>

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

int32_t BlockParser::CodedBlkPattern(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t bits   = 0;
    uint32_t i      = 0;

    do {
	bits = _bitBuffer.PeekBits(9);
	if (7 == (bits >> 8)) {
	    _bitBuffer.GetBits(3);
	    picData->macroblkData.coded_block_pattern_420 = 60;
	} else if (0xC == ((bits >> 5) & 0xC)) {
	    bits = _bitBuffer.GetBits(4);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 4;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 8;
	    }		
	} else if (0xA == ((bits >> 5) & 0xA)) {
	    bits = _bitBuffer.GetBits(4);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 16;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 32;
	    }		
	} else if (0x12 == ((bits >> 4) & 0x12)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 12;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 48;
	    }		
	} else if (0x10 == ((bits >> 4) & 0x10)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 20;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 40;
	    }		
	} else if (0xE == ((bits >> 4) & 0xE)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 28;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 44;
	    }		
	} else if (0xC == ((bits >> 4) & 0xC)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 52;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 56;
	    }		
	} else if (0xA == ((bits >> 4) & 0xA)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 1;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 61;
	    }		
	} else if (0x8 == ((bits >> 4) & 0x8)) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 2;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 62;
	    }		
	} else if (0xE == ((bits >> 3) & 0xE)) {
	    bits = _bitBuffer.GetBits(6);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 24;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 36;
	    }		
	} else if (0xC == ((bits >> 3) & 0xC)) {
	    bits = _bitBuffer.GetBits(6);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 3;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 63;
	    }		
	} else if (0x16 == ((bits >> 2) & 0x16)) {
	    bits = _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 5;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 9;
	    }		
	} else if (0x14 == ((bits >> 2) & 0x14)) {
	    bits = _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 17;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 33;
	    }		
	} else if (0x12 == ((bits >> 2) & 0x12)) {
	    bits = _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 6;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 10;
	    }		
	} else if (0x10 == ((bits >> 2) & 0x10)) {
	    bits = _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 18;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 34;
	    }		
	} else if (0x1E == ((bits >> 1) & 0x1E)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 7;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 11;
	    }		
	} else if (0x1C == ((bits >> 1) & 0x1C)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 19;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 35;
	    }		
	} else if (0x1A == ((bits >> 1) & 0x1A)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 13;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 49;
	    }		
	} else if (0x18 == ((bits >> 1) & 0x18)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 21;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 41;
	    }		
	} else if (0x16 == ((bits >> 1) & 0x16)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 14;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 50;
	    }		
	} else if (0x14 == ((bits >> 1) & 0x14)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 22;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 42;
	    }		
	} else if (0x12 == ((bits >> 1) & 0x12)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 15;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 51;
	    }		
	} else if (0x10 == ((bits >> 1) & 0x10)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 23;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 43;
	    }		
	} else if (0xE == ((bits >> 1) & 0xE)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 25;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 37;
	    }		
	} else if (0xC == ((bits >> 1) & 0xC)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 26;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 38;
	    }		
	} else if (0xA == ((bits >> 1) & 0xA)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 29;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 45;
	    }		
	} else if (0x8 == ((bits >> 1) & 0x8)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 53;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 57;
	    }		
	} else if (0x6 == ((bits >> 1) & 0x6)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 30;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 46;
	    }		
	} else if (0x4 == ((bits >> 1) & 0x4)) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 54;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 58;
	    }		
	} else if (6 == (bits & 6)) {
	    bits = _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 31;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 47;
	    }		
	} else if (4 == (bits & 4)) {
	    bits = _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 55;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 59;
	    }		
	} else if (2 == (bits & 2)) {
	    bits = _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 27;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 39;
	    }		
	} else if (1 == bits) {
	    bits = _bitBuffer.GetBits(9);
	    picData->macroblkData.coded_block_pattern_420 = 0;
	} else {
	    status = -1;
	    break;
	}

	if (sequence_extension::CHROMA_FMT_422 ==
	    _streamState.extData.seqExt.chroma_format) {
	    picData->macroblkData.coded_block_pattern_1 = _bitBuffer.GetBits(2);
	}

	if (sequence_extension::CHROMA_FMT_444 ==
	    _streamState.extData.seqExt.chroma_format) {
	    picData->macroblkData.coded_block_pattern_2 = _bitBuffer.GetBits(6);
	}
    } while(0);

    return status;
}

int32_t BlockParser::ParseBlock(PictureData* picData, uint32_t blkcnt)
{
    int32_t  status = 0;
    int32_t  cc     = 0;
    uint32_t n      = 0;
    
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

	GetPatternCode(picData);

	if (1 == picData->blkData.pattern_code[blkcnt]) {
	    if (1 == picData->macroblkData.macroblock_intra) {
		int32_t dct_diff   = 0;
		int32_t half_range = 0;
		
		if (blkcnt < 4) {
		    status = GetDctSizeLuminance(picData);
		    if (-1 == status) {
			break;
		    }
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
		    status = GetDctSizeChromiance(picData);
		    if (-1 == status) {
			break;
		    }
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

		n = 1;

		int32_t max = (1 << (8 + picData->picCodingExt.intra_dc_prec)) - 1;
		if (0 > picData->blkData.QFS[0] || max < picData->blkData.QFS[0]) {
		    status = -1;
		    break;
		}
	    } else {
		status = ParseFirstDctCoeff(picData, n, blkcnt);
	    }
    
	    //while (_bitBuffer.PeekBits() != EOB) {
		// Subsequent DCT coefficients
	    //}
	    //_bitBuffer.GetBits();  // EOB
	}
    } while (0);

    return status;
}

int32_t BlockParser::GetPatternCode(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t i      = 0;
    uint32_t cbp    = 0;
    uint32_t cbp1   = 0;
    uint32_t cbp2   = 0;

    do {
	for (i = 0; i < 12; i++) {
	    if (1 == picData->macroblkData.macroblock_intra) {
		picData->blkData.pattern_code[i] = 1;
	    } else {
		picData->blkData.pattern_code[i] = 0;
	    }
	}

	cbp  = picData->macroblkData.coded_block_pattern_420;
	cbp1 = picData->macroblkData.coded_block_pattern_1;
	cbp2 = picData->macroblkData.coded_block_pattern_2;
	if (1 == picData->macroblkData.macroblock_pattern) {
	    for (i = 0; i < 6; i++) {
		if (cbp & (1 << (5 - i))) {
		    picData->blkData.pattern_code[i] = 1;
		}
	    }

	    if (sequence_extension::CHROMA_FMT_422 == _streamState.extData.seqExt.chroma_format) {
		for (i = 6; i < 8; i++) {
		    if (cbp1 & (1 << (7 - i))) {
			picData->blkData.pattern_code[i] = 1;
		    }
		}
	    }
	    
	    if (sequence_extension::CHROMA_FMT_444 == _streamState.extData.seqExt.chroma_format) {
		for (i = 6; i < 12; i++) {
		    if (cbp2 & (1 << (11 - i))) {
			picData->blkData.pattern_code[i] = 1;
		    }
		}
	    }
	}
    } while (0);

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
	    if (0 == bits & 1) {
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
	    if (0 == bits & 1) {
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

int32_t BlockParser::GetDctSizeChromiance(PictureData* picData)
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

int32_t BlockParser::ParseFirstDctCoeff(PictureData* picData, uint32_t& n, uint32_t blkcnt)
{
    int32_t status = 0;
    int32_t cc     = 0;
    
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

	
	if (1 == picData->macroblkData.macroblock_intra &&
	    1 == picData->picCodingExt.intra_vlc_format) {
	    // use B.15
	} else {
	    // use B.14
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

