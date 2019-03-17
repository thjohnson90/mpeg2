#include <iostream>
#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "motvecs.h"
#include "block.h"
#include "macroblk.h"

MacroblkParser::MacroblkParser(BitBuffer& bb, StreamState& ss) :
    _macroblkParser(nullptr),
    _motionvecsParser(nullptr),
    _blockParser(nullptr),
    _bitBuffer(bb),
    _streamState(ss)
{
}

MacroblkParser::~MacroblkParser()
{
    Destroy();
}

int32_t MacroblkParser::Initialize(void)
{
    int32_t status = 0;

    do {
	// create parsers
	if (nullptr == _motionvecsParser) {
	    try {
		_motionvecsParser = GetMotionVecsParser();
		if (nullptr != _motionvecsParser) {
		    _motionvecsParser->Initialize();
		}

		_blockParser = GetBlockParser();
		if (nullptr != _blockParser) {
		    _blockParser->Initialize();
		}
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}
    } while (0);
    
    return status;
}

int32_t MacroblkParser::Destroy(void)
{
    int32_t status = 0;

    do {
	if (nullptr != _motionvecsParser) {
	    _motionvecsParser->Destroy();
	}
	delete _motionvecsParser;
	_motionvecsParser = nullptr;

	if (nullptr != _blockParser) {
	    _blockParser->Destroy();
	}
	delete _blockParser;
	_blockParser = nullptr;
    } while (0);
    
    return status;
}

int32_t MacroblkParser::ParseMacroblkData(void)
{
    int32_t      status  = 0;
    uint32_t     marker  = 0;
    uint32_t     esccnt  = 0;
    PictureData* picData = 0;
    
    do {
	uint32_t bitCnt = 0;
	uint32_t escape = 0;
	
        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        
        picData = picDataMgr->GetCurrentBuffer();
        if (0 == picData) {
            status = -1;
            break;
        }

	while (8 == _bitBuffer.PeekBits(11)) {
	    escape = _bitBuffer.GetBits(11);
	    esccnt++;
	}

	picData->macroblkData.macroblock_address_inc = GetMacroblkAddrInc();
	picData->macroblkData.macroblock_address =
	    picData->macroblkData.previous_macroblock_address +
	    picData->macroblkData.macroblock_address_inc +
	    (33 * esccnt);
	picData->macroblkData.mb_col =
	    picData->macroblkData.macroblock_address % picData->macroblkData.mb_width;
	
	status = GetMacroblkModes(picData);

	if (1 != picData->macroblkData.macroblock_intra ||
	    1 < picData->macroblkData.macroblock_address_inc) {
	    	picData->ResetDctDcPred();
	}

	if (1 == picData->macroblkData.macroblock_quant) {
	    picData->macroblkData.quantiser_scale_code = _bitBuffer.GetBits(5);
	}

	if (1 == picData->macroblkData.macroblock_motion_forw ||
	    (1 == picData->macroblkData.macroblock_intra &&
	     picData->picCodingExt.concealment_mot_vecs)) {
	    _motionvecsParser->ParseMotionVecs(picData, 0);
	}

	if (1 == picData->macroblkData.macroblock_motion_back) {
	    _motionvecsParser->ParseMotionVecs(picData, 1);
	}

	if (1 == picData->macroblkData.macroblock_intra &&
	    picData->picCodingExt.concealment_mot_vecs) {
	    marker = _bitBuffer.GetBits(1);
	}

	if (1 == picData->macroblkData.macroblock_pattern) {
	    _blockParser->CodedBlkPattern(picData);
	}

	// get the block count from the chroma format
	switch(_streamState.extData.seqExt.chroma_format) {
	case sequence_extension::CHROMA_FMT_420:
	    picData->macroblkData.block_count = 6;
	    break;

	case sequence_extension::CHROMA_FMT_422:
	    picData->macroblkData.block_count = 8;
	    break;

	case sequence_extension::CHROMA_FMT_444:
	    picData->macroblkData.block_count = 12;
	    break;

	default:
	    picData->macroblkData.block_count = 0;
	    status = -1;
	    break;
	}

	for (uint32_t i = 0; i < picData->macroblkData.block_count; i++) {
	    _blockParser->ParseBlock(picData, i);
	}
    } while (0);
    
    return status;
}

MacroblkParser* MacroblkParser::GetMacroblkParser(void)
{
    if (nullptr == _macroblkParser) {
        _macroblkParser = new MacroblkParser(_bitBuffer, _streamState);
    }
    return _macroblkParser;
}

MotionVecsParser* MacroblkParser::GetMotionVecsParser(void)
{
    if (nullptr == _motionvecsParser) {
        _motionvecsParser = new MotionVecsParser(_bitBuffer, _streamState);
    }
    return _motionvecsParser;
}

BlockParser* MacroblkParser::GetBlockParser(void)
{
    if (nullptr == _blockParser) {
	_blockParser = new BlockParser(_bitBuffer, _streamState);
    }
    return _blockParser;
}

uint32_t MacroblkParser::GetMacroblkAddrInc(void)
{
    uint32_t addrInc = 0;
    uint32_t bits    = 0;

    do {
	bits = _bitBuffer.PeekBits(11);
	
	if (1 == bits >> 10)
	{
	    _bitBuffer.GetBits(1);
	    addrInc = 1;
	} else if (1 == bits >> 9) {
	    bits = _bitBuffer.GetBits(3);
	    if (1 == (bits & 1)) {
		addrInc = 2;
	    } else {
		addrInc = 3;
	    }
	} else if (1 == bits >> 8) {
	    bits = _bitBuffer.GetBits(4);
	    if (1 == (bits & 1)) {
		addrInc = 4;
	    } else {
		addrInc = 5;
	    }
	} else if (1 == bits >> 7) {
	    bits = _bitBuffer.GetBits(5);
	    if (1 == (bits & 1)) {
		addrInc = 6;
	    } else {
		addrInc = 7;
	    }
	} else if (3 == bits >> 5) {
	    bits = _bitBuffer.GetBits(7);
	    if (1 == (bits & 1)) {
		addrInc = 8;
	    } else {
		addrInc = 9;
	    }
	} else if (5 == bits >> 4) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == (bits & 1)) {
		addrInc = 10;
	    } else {
		addrInc = 11;
	    }
	} else if (4 == bits >> 4) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == (bits & 1)) {
		addrInc = 12;
	    } else {
		addrInc = 13;
	    }
	} else if (3 == bits >> 4) {
	    bits = _bitBuffer.GetBits(8);
	    if (1 == (bits & 1)) {
		addrInc = 14;
	    } else {
		addrInc = 15;
	    }
	} else if (0xB == bits >> 2) {
	    bits = _bitBuffer.GetBits(10);
	    if (1 == (bits & 1)) {
		addrInc = 16;
	    } else {
		addrInc = 17;
	    }
	} else if (0xA == bits >> 2) {
	    bits = _bitBuffer.GetBits(10);
	    if (1 == (bits & 1)) {
		addrInc = 18;
	    } else {
		addrInc = 19;
	    }
	} else if (9 == bits >> 2) {
	    bits = _bitBuffer.GetBits(10);
	    if (1 == (bits & 1)) {
		addrInc = 20;
	    } else {
		addrInc = 21;
	    }
	} else if (0x11 == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 22;
	    } else {
		addrInc = 23;
	    }
	} else if (0x10 == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 24;
	    } else {
		addrInc = 25;
	    }
	} else if (0xF == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 26;
	    } else {
		addrInc = 27;
	    }
	} else if (0xE == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 28;
	    } else {
		addrInc = 29;
	    }
	} else if (0xD == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 30;
	    } else {
		addrInc = 31;
	    }
	} else if (0xC == bits >> 1) {
	    bits = _bitBuffer.GetBits(11);
	    if (1 == (bits & 1)) {
		addrInc = 32;
	    } else {
		addrInc = 33;
	    }
	}
    } while (0);

    return addrInc;
}

int32_t MacroblkParser::GetMacroblkModes(PictureData* picData)
{
    uint32_t status = 0;

    do {
	switch (picData->picHdr.picture_coding_type) {
	case PictureHeader::PIC_CODING_TYPE_I:
	    status = MbModeIPic(picData);
	    break;
	    
	case PictureHeader::PIC_CODING_TYPE_P:
	    status = MbModePPic(picData);
	    break;
	    
	case PictureHeader::PIC_CODING_TYPE_B:
	    status = MbModeBPic(picData);
	    break;
	    
	default:
	    cout << "Invalid picture coding type 0x" << hex << picData->picHdr.picture_coding_type << endl;
	    status = -1;
	    break;
	}

	if (-1 == status) {
	  break;
	}

#ifndef TEST
	if ((1 == picData->macroblkData.spatial_temporal_weight_code_flag) &&
	    (0 != _streamState.extData.pictSpatScalExt.spat_temp_wt_cd_tbl_idx)) {
	    picData->macroblkData.spatial_temporal_weight_code = _bitBuffer.GetBits(2);
	}

	if (1 == picData->macroblkData.macroblock_motion_forw ||
	    1 == picData->macroblkData.macroblock_motion_back) {
	    if (PictureCodingExtension::PIC_STRUCT_FRAME == picData->picCodingExt.picture_struct) {
		if (0 == picData->picCodingExt.frame_pred_frame_dct) {
		    picData->macroblkData.frame_motion_type = _bitBuffer.GetBits(2);
		}
	    } else {
		picData->macroblkData.field_motion_type = _bitBuffer.GetBits(2);
	    }
	}

	if (PictureCodingExtension::PIC_STRUCT_FRAME == picData->picCodingExt.picture_struct &&
	    0 == picData->picCodingExt.frame_pred_frame_dct &&
	    (1 == picData->macroblkData.macroblock_intra || 1 == picData->macroblkData.macroblock_pattern)) {
	    picData->macroblkData.dct_type = _bitBuffer.GetBits(1);
	}
#endif
    } while (0);

    return status;
}

int32_t MacroblkParser::MbModeIPic(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t bits   = 0;

    do {
	if (sequence_scalable_extension::spatial_scalability ==
	    _streamState.extData.seqScalExt.scalable_mode) {
	    bits = _bitBuffer.PeekBits(4);
	    if (1 == bits >> 3) {
		_bitBuffer.GetBits(1);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (1 == bits >> 2) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (3 == bits) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else {
		status = -1;
		break;
	    }
	} else {
	    bits = _bitBuffer.PeekBits(2);
	    if (1 == bits >> 1) {
		_bitBuffer.GetBits(1);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else {
		status = -1;
		break;
	    }
	}
    } while (0);

    return status;
}

int32_t MacroblkParser::MbModePPic(PictureData* picData)
{
    int32_t  status = 0;
    uint32_t bits   = 0;
    
    do {
	if (sequence_scalable_extension::spatial_scalability ==
	    _streamState.extData.seqScalExt.scalable_mode) {
	    bits = _bitBuffer.PeekBits(7);
	    if (2 == bits >> 5) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 4) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (4 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (7 == bits >> 1) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (2 == bits >> 3) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (7 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 3) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (2 == bits >> 4) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (4 == bits >> 1) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (6 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 5) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (5 == bits >> 1) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (6 == bits >> 1) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (5 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (2 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (3 == bits) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else {
		status = -1;
		break;
	    }
	} else {
	    bits = _bitBuffer.PeekBits(6);
	    if (1 == bits >> 5) {
		_bitBuffer.GetBits(1);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits >> 4) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits >> 3) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 1) {
		_bitBuffer.GetBits(5);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 1) {
		_bitBuffer.GetBits(5);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits >> 1) {
		_bitBuffer.GetBits(5);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else {
		status = -1;
		break;
	    }
	}
    } while (0);

    return status;
}

int32_t MacroblkParser::MbModeBPic(PictureData* picData)
{
    int32_t status = 0;
    uint32_t bits  = 0;

    do {
	if (sequence_scalable_extension::spatial_scalability ==
	    _streamState.extData.seqScalExt.scalable_mode) {
	    bits = _bitBuffer.PeekBits(9);
	    if (2 == bits >> 7) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 7) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 6) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 6) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 5) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 5) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (6 == bits >> 3) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (7 == bits >> 3) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (4 == bits >> 3) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (5 == bits >> 3) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (6 == bits >> 2) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (7 == bits >> 2) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (4 == bits >> 2) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (5 == bits >> 2) {
		_bitBuffer.GetBits(7);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (4 == bits >> 1) {
		_bitBuffer.GetBits(8);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (5 == bits >> 1) {
		_bitBuffer.GetBits(8);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (0xC == bits) {
		_bitBuffer.GetBits(9);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 1;
	    } else if (0xE == bits) {
		_bitBuffer.GetBits(9);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (0xD == bits) {
		_bitBuffer.GetBits(9);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else if (0xF == bits) {
		_bitBuffer.GetBits(9);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
		picData->macroblkData.spatial_temporal_weight_class     = 4;
	    } else {
		status = -1;
		break;
	    }
	} else {
	    bits = _bitBuffer.PeekBits(6);
	    if (2 == bits >> 4) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 4) {
		_bitBuffer.GetBits(2);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 3) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 3) {
		_bitBuffer.GetBits(3);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 2) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 2) {
		_bitBuffer.GetBits(4);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits >> 1) {
		_bitBuffer.GetBits(5);
		picData->macroblkData.macroblock_quant                  = 0;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits >> 1) {
		_bitBuffer.GetBits(5);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (3 == bits) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 1;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (2 == bits) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 1;
		picData->macroblkData.macroblock_pattern                = 1;
		picData->macroblkData.macroblock_intra                  = 0;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else if (1 == bits) {
		_bitBuffer.GetBits(6);
		picData->macroblkData.macroblock_quant                  = 1;
		picData->macroblkData.macroblock_motion_forw            = 0;
		picData->macroblkData.macroblock_motion_back            = 0;
		picData->macroblkData.macroblock_pattern                = 0;
		picData->macroblkData.macroblock_intra                  = 1;
		picData->macroblkData.spatial_temporal_weight_code_flag = 0;
	    } else {
		status = -1;
		break;
	    }
	}
    } while (0);

    return status;
}
