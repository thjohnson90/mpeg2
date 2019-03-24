#include <iostream>
#include <stdint.h>
#include <assert.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "pack.h"
#include "syshdr.h"
#include "peshdr.h"
#include "sequence.h"
#include "extension.h"
#include "user.h"
#include "gop.h"
#include "picdata.h"
#include "motvecs.h"
#include "videoproc.h"
#include "block.h"
#include "macroblk.h"
#include "slice.h"
#include "picture.h"
#include "doorbell.h"
#include "thread.h"
#include "base_parser.h"
#include "file_bitbuf.h"
#include "buf_bitbuf.h"

PictureParser::PictureParser(BitBuffer& bb, StreamState& ss) :
    _sliceParser(nullptr),
    _bitBuffer(bb),
    _streamState(ss)
{
}

PictureParser::~PictureParser()
{
    Destroy();
}

int32_t PictureParser::Initialize(void)
{
    int32_t status = 0;

    do {
	// create parsers
	if (nullptr == _sliceParser) {
	    try {
		_sliceParser = GetSliceParser();
		if (nullptr != _sliceParser) {
		    _sliceParser->Initialize();
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

int32_t PictureParser::Destroy(void)
{
    int32_t status = 0;

    do {
	if (nullptr != _sliceParser) {
	    _sliceParser->Destroy();
	}
	delete _sliceParser;
	_sliceParser = nullptr;
    } while (0);

    return status;
}

int32_t PictureParser::ParsePictureHdr(void)
{
    int32_t  status  = 0;
    uint32_t horz_sz = 0;
    
    do {
        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        PictureData* picData = picDataMgr->GetNextBuffer();
        if (0 == picData) {
            status = -1;
            break;
        }
        
        picData->picHdr.temporal_reference  = _bitBuffer.GetBits(10);
        picData->picHdr.picture_coding_type = _bitBuffer.GetBits(3);
        picData->picHdr.vbv_delay           = _bitBuffer.GetBits(16);

	picData->macroblkData.macroblock_address          = 0;
	picData->macroblkData.previous_macroblock_address = 0;
	picData->macroblkData.slice_vertical_pos          = 0;
	picData->macroblkData.mb_row                      = 0;
	picData->macroblkData.mb_col                      = 0;
	picData->macroblkData.mb_width                    = 0;

	horz_sz = static_cast<uint32_t>(12 << _streamState.extData.seqExt.horz_sz_ext) +
	    static_cast<uint32_t>(_streamState.seqHdr.horizontal_sz);
	picData->macroblkData.mb_width = (horz_sz + 15) / 16;
	
        if (picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_P ||
            picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_B) {
            picData->picHdr.full_pel_forw_vector = _bitBuffer.GetBits(1);
            picData->picHdr.forw_f_code = _bitBuffer.GetBits(3);
        }
        
        if (picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_B) {
            picData->picHdr.full_pel_back_vector = _bitBuffer.GetBits(1);
            picData->picHdr.back_f_code = _bitBuffer.GetBits(3);
        }
        
        while (1 == _bitBuffer.PeekBits(1)) {
	    picData->picHdr.extra_bit_pict  = _bitBuffer.GetBits(1);
            picData->picHdr.extra_info_pict = _bitBuffer.GetBits(8);
	    // TODO: do something with extra_info_pict data
        }
	if (0 > status) {
	    break;
	}

	picData->picHdr.extra_bit_pict = _bitBuffer.GetBits(1);

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

int32_t PictureParser::ParsePictCodingExt(void)
{
    int32_t  status = 0;
    
    do {
        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        PictureData* picData = picDataMgr->GetBackBuffer();
	if (0 == picData) {
	    status = -1;
	    break;
	}

	// get the extension start code identifier
	uint8_t sc = _bitBuffer.GetBits(4);
	assert(PictureParser::PICT_CODING_EXT_ID == sc);
	
        picData->picCodingExt.f_code[0][0]          = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code[0][1]          = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code[1][0]          = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code[1][1]          = _bitBuffer.GetBits(4);
        picData->picCodingExt.intra_dc_prec         = _bitBuffer.GetBits(2);
        picData->picCodingExt.picture_struct        = _bitBuffer.GetBits(2);
        picData->picCodingExt.top_field_first       = _bitBuffer.GetBits(1);
        picData->picCodingExt.frame_pred_frame_dct  = _bitBuffer.GetBits(1);
        picData->picCodingExt.concealment_mot_vecs  = _bitBuffer.GetBits(1);
        picData->picCodingExt.q_scale_type          = _bitBuffer.GetBits(1);
        picData->picCodingExt.intra_vlc_format      = _bitBuffer.GetBits(1);
        picData->picCodingExt.alternate_scan        = _bitBuffer.GetBits(1);
        picData->picCodingExt.repeat_first_field    = _bitBuffer.GetBits(1);
        picData->picCodingExt.chroma_420_type       = _bitBuffer.GetBits(1);
        picData->picCodingExt.progressive_frame     = _bitBuffer.GetBits(1);
        picData->picCodingExt.composite_display     = _bitBuffer.GetBits(1);
        if (1 == picData->picCodingExt.composite_display) {
            picData->picCodingExt.v_axis            = _bitBuffer.GetBits(1);
            picData->picCodingExt.field_sequence    = _bitBuffer.GetBits(3);
            picData->picCodingExt.sub_carrier       = _bitBuffer.GetBits(1);
            picData->picCodingExt.burst_amplitude   = _bitBuffer.GetBits(7);
            picData->picCodingExt.sub_carrier_phase = _bitBuffer.GetBits(8);
        }

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

int32_t PictureParser::ParsePictData(void)
{
    int32_t status = 0;
    
    do {
	do {
	    status = _sliceParser->ParseSliceData();
	    if (-1 == status) {
		break;
	    }
	} while ((StreamState::slice_start_min < _bitBuffer.GetLastStartCode()) &&
		 (StreamState::slice_start_max >= _bitBuffer.GetLastStartCode()));
    } while (0);

    return status;
}

SliceParser* PictureParser::GetSliceParser(void)
{
    if (nullptr == _sliceParser) {
        _sliceParser = new SliceParser(_bitBuffer, _streamState);
    }
    return _sliceParser;
}
