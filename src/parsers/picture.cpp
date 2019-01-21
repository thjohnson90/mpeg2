#include <iostream>
#include <stdint.h>
#include <assert.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picture.h"
#include "picdata.h"

PictureParser::PictureParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

uint32_t PictureParser::ParsePictureHdr(void)
{
    uint32_t     status  = 0;
    uint32_t     marker  = 0;
    
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
        
        if (picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_P ||
            picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_B) {
            picData->picHdr.full_pel_forw_vector = _bitBuffer.GetBits(1);
            picData->picHdr.forw_f_code = _bitBuffer.GetBits(3);
        }
        
        if (picData->picHdr.picture_coding_type == PictureHeader::PIC_CODING_TYPE_B) {
            picData->picHdr.full_pel_back_vector = _bitBuffer.GetBits(1);
            picData->picHdr.back_f_code = _bitBuffer.GetBits(3);
        }
        
        while (1 == _bitBuffer.PeekBits(1, status) && 0 <= status) {
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

uint32_t PictureParser::ParsePictCodingExt(void)
{
    uint32_t status = 0;
    uint32_t marker = 0;
    
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
	
        picData->picCodingExt.f_code_forw_horz      = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code_forw_vert      = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code_back_horz      = _bitBuffer.GetBits(4);
        picData->picCodingExt.f_code_back_vert      = _bitBuffer.GetBits(4);
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

uint32_t PictureParser::ParsePictData(void)
{
    uint32_t status = 0;
    uint8_t  sc     = 0;

    do {
	do {
	    // slice()

	    sc = _bitBuffer.GetNextStartCode();
	} while ((StreamState::slice_start_min < sc) &&
		  (StreamState::slice_start_max >= sc));
//	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}
