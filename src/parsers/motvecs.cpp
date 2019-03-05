#include <iostream>
#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "motvecs.h"
#include "macroblk.h"

MotionVecsParser::MotionVecsParser(BitBuffer& bb, StreamState& ss) :
    _motionvecsParser(nullptr), _bitBuffer(bb), _streamState(ss)
{
}

MotionVecsParser::~MotionVecsParser()
{
    Destroy();
}

int32_t MotionVecsParser::Initialize(void)
{
    int32_t status = 0;

#if 0
    do {
      // create parsers
	if (nullptr == _motionvecsParser) {
	    try {
		_motionvecsParser = GetMotionVectorsParser();
		if (nullptr != _motionvecsParser) {
		    _motionvecsParser->Initalize();
		}
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}
    } while (0);
#endif
    
    return status;
}

int32_t MotionVecsParser::Destroy(void)
{
    int32_t status = 0;

#if 0
    do {
      if (nullptr != _motionvecsParser) {
	    _motionvecsParser->Destroy();
	}
	delete _motionvecsParser;
	_motionvecsParser = nullptr;
    } while (0);
#endif
    
    return status;
}

int32_t MotionVecsParser::ParseMotionVecs(PictureData* picData, uint32_t s)
{
    int32_t  status  = 0;
    uint32_t marker  = 0;

    do {
	uint32_t bitCnt = 0;
	uint32_t mvcnt  = 0;

	if (0 == picData->macroblkData.spatial_temporal_weight_code) {
	    picData->macroblkData.spatial_temporal_integer_weight = 1;
	} else {
	    picData->macroblkData.spatial_temporal_integer_weight = 0;
	}
	
	switch(_streamState.extData.pictSpatScalExt.spat_temp_wt_cd_tbl_idx) {
	case 0:
	    picData->macroblkData.spatial_temporal_weight_class   = 1;
	    picData->macroblkData.spatial_temporal_integer_weight = 0;
	    break;
	    
	case 1:
	    if (0 == picData->macroblkData.spatial_temporal_weight_code & 1) {
		picData->macroblkData.spatial_temporal_weight_class = 3;
	    } else {
		picData->macroblkData.spatial_temporal_weight_class = 1;
	    }
	    break;
	    
	case 2:
	    if (0 == picData->macroblkData.spatial_temporal_weight_code & 1) {
		picData->macroblkData.spatial_temporal_weight_class = 2;
	    } else {
		picData->macroblkData.spatial_temporal_weight_class = 1;
	    }
	    break;

	case 3:
	    if (0 == picData->macroblkData.spatial_temporal_weight_code & 2) {
		picData->macroblkData.spatial_temporal_weight_class = 2;
	    } else if (2 == picData->macroblkData.spatial_temporal_weight_code) {
		picData->macroblkData.spatial_temporal_weight_class = 3;
	    } else {
		picData->macroblkData.spatial_temporal_weight_class = 1;
	    }		
	    break;
	}

	if (0 != picData->macroblkData.frame_motion_type) {
	    picData->macroblkData.motion_vector_count = 1;
	    picData->macroblkData.mv_format           = MacroblkData::MVFMT_FIELD;
	    picData->macroblkData.dmv                 = 0;
	    if (1 == picData->macroblkData.frame_motion_type) {
		if (0 == picData->macroblkData.spatial_temporal_weight_class ||
		    1 == picData->macroblkData.spatial_temporal_weight_class) {
		    picData->macroblkData.motion_vector_count = 2;
		}
	    }
	    if (2 == picData->macroblkData.frame_motion_type) {
		if (4 != picData->macroblkData.spatial_temporal_weight_class) {
		    picData->macroblkData.mv_format = MacroblkData::MVFMT_FRAME;
		}
	    }
	    if (3 == picData->macroblkData.frame_motion_type) {
		if (4 != picData->macroblkData.spatial_temporal_weight_class) {
		    picData->macroblkData.dmv = 1;
		}
	    }
	}
	
	if (0 != picData->macroblkData.field_motion_type) {
	    picData->macroblkData.motion_vector_count = 1;
	    picData->macroblkData.mv_format           = MacroblkData::MVFMT_FIELD;
	    picData->macroblkData.dmv                 = 0;
	    if (2 == picData->macroblkData.field_motion_type) {
		picData->macroblkData.motion_vector_count = 2;
	    }
	    if (3 == picData->macroblkData.field_motion_type) {
		picData->macroblkData.dmv = 1;
	    }
	}

	if (1 == picData->macroblkData.motion_vector_count) {
	    if (MacroblkData::MVFMT_FIELD == picData->macroblkData.mv_format &&
		1 != picData->macroblkData.dmv) {
		picData->macroblkData.motion_vertical_field_select[0][s] = _bitBuffer.GetBits(1);
	    }
	    ParseMotionVec(picData, 0, s);
	} else {
	    picData->macroblkData.motion_vertical_field_select[0][s] = _bitBuffer.GetBits(1);
	    ParseMotionVec(picData, 0, s);
	    picData->macroblkData.motion_vertical_field_select[1][s] = _bitBuffer.GetBits(1);
	    ParseMotionVec(picData, 1, s);
	}
    } while (0);
    
    return status;
}

int32_t MotionVecsParser::ParseMotionVec(PictureData* picData, uint32_t r, uint32_t s)
{
    int32_t status = 0;

    do {
	status = GetMotionCode(picData, r, s, 0);
	if (-1 == status) {
	    break;
	}
    } while (0);
    
    return status;
}

MotionVecsParser* MotionVecsParser::GetMotionVecsParser(void)
{
    if (nullptr == _motionvecsParser) {
        _motionvecsParser = new MotionVecsParser(_bitBuffer, _streamState);
    }
    return _motionvecsParser;
}

int32_t MotionVecsParser::GetMotionCode(PictureData* picData, uint32_t r, uint32_t s, uint32_t t)
{
    uint32_t status = 0;
    uint32_t mc     = 0;

    do {
	mc = _bitBuffer.PeekBits(11);

	if (1 == (mc >> 10)) {
	    picData->mvData.motion_code[r][s][t] = 0;
	} else if (2 == ((mc >> 8) & 2)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -1;
	    } else {
		picData->mvData.motion_code[r][s][t] = 1;
	    }
	} else if (2 == ((mc >> 7) & 2)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -2;
	    } else {
		picData->mvData.motion_code[r][s][t] = 2;
	    }
	} else if (2 == ((mc >> 6) & 2)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -3;
	    } else {
		picData->mvData.motion_code[r][s][t] = 3;
	    }
	} else if (6 == ((mc >> 4) & 6)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -4;
	    } else {
		picData->mvData.motion_code[r][s][t] = 4;
	    }
	} else if (0xA == ((mc >> 3) & 0xA)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -5;
	    } else {
		picData->mvData.motion_code[r][s][t] = 5;
	    }
	} else if (8 == ((mc >> 3) & 8)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -6;
	    } else {
		picData->mvData.motion_code[r][s][t] = 6;
	    }
	} else if (6 == ((mc >> 3) & 6)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -7;
	    } else {
		picData->mvData.motion_code[r][s][t] = 7;
	    }
	} else if (0x16 == ((mc >> 1) & 0x16)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -8;
	    } else {
		picData->mvData.motion_code[r][s][t] = 8;
	    }
	} else if (0x14 == ((mc >> 1) & 0x14)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -9;
	    } else {
		picData->mvData.motion_code[r][s][t] = 9;
	    }
	} else if (0x12 == ((mc >> 1) & 0x12)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -10;
	    } else {
		picData->mvData.motion_code[r][s][t] = 10;
	    }
	} else if (0x22 == (mc & 0x22)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -11;
	    } else {
		picData->mvData.motion_code[r][s][t] = 11;
	    }
	} else if (0x20 == (mc & 0x20)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -12;
	    } else {
		picData->mvData.motion_code[r][s][t] = 12;
	    }
	} else if (0x1E == (mc & 0x1E)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -13;
	    } else {
		picData->mvData.motion_code[r][s][t] = 13;
	    }
	} else if (0x1C == (mc & 0x1C)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -14;
	    } else {
		picData->mvData.motion_code[r][s][t] = 14;
	    }
	} else if (0x1A == (mc & 0x1A)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -15;
	    } else {
		picData->mvData.motion_code[r][s][t] = 15;
	    }
	} else if (0x18 == (mc & 0x18)) {
	    if (1 == mc & 1) {
		picData->mvData.motion_code[r][s][t] = -16;
	    } else {
		picData->mvData.motion_code[r][s][t] = 16;
	    }
	} else {
	    status = -1;
	    break;
	}
    } while (0);

    return status;
}

