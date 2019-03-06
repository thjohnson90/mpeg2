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
	    _bitBuffer.GetBits(4);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 4;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 8;
	    }		
	} else if (0xA == ((bits >> 5) & 0xA)) {
	    _bitBuffer.GetBits(4);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 16;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 32;
	    }		
	} else if (0x12 == ((bits >> 4) & 0x12)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 12;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 48;
	    }		
	} else if (0x10 == ((bits >> 4) & 0x10)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 20;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 40;
	    }		
	} else if (0xE == ((bits >> 4) & 0xE)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 28;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 44;
	    }		
	} else if (0xC == ((bits >> 4) & 0xC)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 52;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 56;
	    }		
	} else if (0xA == ((bits >> 4) & 0xA)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 1;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 61;
	    }		
	} else if (0x8 == ((bits >> 4) & 0x8)) {
	    _bitBuffer.GetBits(5);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 2;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 62;
	    }		
	} else if (0xE == ((bits >> 3) & 0xE)) {
	    _bitBuffer.GetBits(6);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 24;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 36;
	    }		
	} else if (0xC == ((bits >> 3) & 0xC)) {
	    _bitBuffer.GetBits(6);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 3;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 63;
	    }		
	} else if (0x16 == ((bits >> 2) & 0x16)) {
	    _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 5;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 9;
	    }		
	} else if (0x14 == ((bits >> 2) & 0x14)) {
	    _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 17;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 33;
	    }		
	} else if (0x12 == ((bits >> 2) & 0x12)) {
	    _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 6;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 10;
	    }		
	} else if (0x10 == ((bits >> 2) & 0x10)) {
	    _bitBuffer.GetBits(7);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 18;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 34;
	    }		
	} else if (0x1E == ((bits >> 1) & 0x1E)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 7;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 11;
	    }		
	} else if (0x1C == ((bits >> 1) & 0x1C)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 19;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 35;
	    }		
	} else if (0x1A == ((bits >> 1) & 0x1A)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 13;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 49;
	    }		
	} else if (0x18 == ((bits >> 1) & 0x18)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 21;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 41;
	    }		
	} else if (0x16 == ((bits >> 1) & 0x16)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 14;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 50;
	    }		
	} else if (0x14 == ((bits >> 1) & 0x14)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 22;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 42;
	    }		
	} else if (0x12 == ((bits >> 1) & 0x12)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 15;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 51;
	    }		
	} else if (0x10 == ((bits >> 1) & 0x10)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 23;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 43;
	    }		
	} else if (0xE == ((bits >> 1) & 0xE)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 25;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 37;
	    }		
	} else if (0xC == ((bits >> 1) & 0xC)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 26;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 38;
	    }		
	} else if (0xA == ((bits >> 1) & 0xA)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 29;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 45;
	    }		
	} else if (0x8 == ((bits >> 1) & 0x8)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 53;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 57;
	    }		
	} else if (0x6 == ((bits >> 1) & 0x6)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 30;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 46;
	    }		
	} else if (0x4 == ((bits >> 1) & 0x4)) {
	    _bitBuffer.GetBits(8);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 54;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 58;
	    }		
	} else if (6 == (bits & 6)) {
	    _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 31;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 47;
	    }		
	} else if (4 == (bits & 4)) {
	    _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 55;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 59;
	    }		
	} else if (2 == (bits & 2)) {
	    _bitBuffer.GetBits(9);
	    if (1 == bits & 1) {
		picData->macroblkData.coded_block_pattern_420 = 27;
	    } else {
		picData->macroblkData.coded_block_pattern_420 = 39;
	    }		
	} else if (1 == bits) {
	    _bitBuffer.GetBits(9);
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

	GetPatternCode(picData);
    } while(0);

    return status;
}

int32_t BlockParser::ParseBlock(PictureData* picData, uint32_t blkcnt)
{
    return 0;
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
