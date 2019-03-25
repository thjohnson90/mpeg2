#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "extension.h"
#include "picdata.h"

extern const int32_t quant_mtx_sz;

SeqExtParser::SeqExtParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t SeqExtParser::ParseSequenceExt(void)
{
    int32_t status = 0;
    int8_t  sc     = 0;
    
    do {
	sc = _bitBuffer.GetBits(4);
	assert(SeqExtParser::seq_ext == sc);
	
        _streamState.extData.seqExt.profile_level    = _bitBuffer.GetBits(8);
        _streamState.extData.seqExt.progressive_seq  = _bitBuffer.GetBits(1);
        _streamState.extData.seqExt.chroma_format    = _bitBuffer.GetBits(2);
        _streamState.extData.seqExt.horz_sz_ext      = _bitBuffer.GetBits(2);
        _streamState.extData.seqExt.vert_sz_ext      = _bitBuffer.GetBits(2);
        _streamState.extData.seqExt.bit_rate_ext     = _bitBuffer.GetBits(12);
        _streamState.extData.seqExt.vbv_buf_sz_ext   = _bitBuffer.GetBits(8);
        _streamState.extData.seqExt.low_delay        = _bitBuffer.GetBits(1);
        _streamState.extData.seqExt.frame_rate_ext_n = _bitBuffer.GetBits(2);
        _streamState.extData.seqExt.frame_rate_ext_d = _bitBuffer.GetBits(5);

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

int32_t SeqExtParser::ParseExtensionData(uint32_t flag)
{
    int32_t  status = 0;
    uint8_t  sc     = 0;
    
    do {
	while (_bitBuffer.GetLastStartCode() == StreamState::extension_start) {
	    sc = _bitBuffer.PeekBits(4);

	    if (seq_display_ext != sc &&
		seq_scalable_ext != sc &&
		quant_matrix_ext != sc &&
		copyright_ext != sc &&
		pict_display_ext != sc &&
		pict_spatial_scalable_ext != sc &&
		pict_temporal_scalable_ext != sc) {
		break;
	    }

	    if (0 == flag) {
		sc = _bitBuffer.GetBits(ext_start_code_id_size);
		if (seq_display_ext == sc) {
		    CHECK_PARSE(ParseSeqDisplayExt(), status);
		} else {
		    assert(sc == seq_scalable_ext);
		    CHECK_PARSE(ParseSeqScalableExt(), status);
		}
	    }
	    
	    if (2 == flag) {
		sc = _bitBuffer.GetBits(ext_start_code_id_size);
		if (quant_matrix_ext == sc) {
		    CHECK_PARSE(ParseQuantMatrixExt(), status);
		} else if (copyright_ext == sc) {
		    CHECK_PARSE(ParseCopyrightExt(), status);
		} else if (pict_display_ext == sc) {
		    CHECK_PARSE(ParsePictureDispExt(), status);
		} else if (pict_spatial_scalable_ext) {
		    CHECK_PARSE(ParsePictSpatialScalExt(), status);
		} else {
		    assert(pict_temporal_scalable_ext == sc);
		    CHECK_PARSE(ParsePictTemporalScalExt(), status);
		}
	    }
	}
    } while (0);
    
    return status;
}

int32_t SeqExtParser::ParseSeqDisplayExt(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    
    do {
        _streamState.extData.seqDispExt.video_format        = _bitBuffer.GetBits(3);
        _streamState.extData.seqDispExt.color_desc_flag     = _bitBuffer.GetBits(1);
        if (1 == _streamState.extData.seqDispExt.color_desc_flag) {
            _streamState.extData.seqDispExt.color_primaries = _bitBuffer.GetBits(8);
            _streamState.extData.seqDispExt.transfer_char   = _bitBuffer.GetBits(8);
            _streamState.extData.seqDispExt.matrix_coeff    = _bitBuffer.GetBits(8);
        }
        _streamState.extData.seqDispExt.display_horz_sz     = _bitBuffer.GetBits(14);
        marker                                  = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.extData.seqDispExt.display_vert_sz     = _bitBuffer.GetBits(14);
        marker                                  = _bitBuffer.GetBits(3);
        CHECK_VALUE_AND_BREAK(0, marker, status, -1);

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

int32_t SeqExtParser::ParseSeqScalableExt(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;

    do {
	_streamState.extData.seqScalExt.present       = true;
	_streamState.extData.seqScalExt.scalable_mode = _bitBuffer.GetBits(2);
	_streamState.extData.seqScalExt.layer_id      = _bitBuffer.GetBits(4);

	if (sequence_scalable_extension::spatial_scalability ==
	    _streamState.extData.seqScalExt.scalable_mode) {
	    _streamState.extData.seqScalExt.lwr_lyr_pred_horz_sz = _bitBuffer.GetBits(14);
	    marker = _bitBuffer.GetBits(1);
	    CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	    _streamState.extData.seqScalExt.lwr_lyr_pred_vert_sz = _bitBuffer.GetBits(14);

	    _streamState.extData.seqScalExt.horz_subsmp_fact_m = _bitBuffer.GetBits(5);
	    _streamState.extData.seqScalExt.horz_subsmp_fact_n = _bitBuffer.GetBits(5);
	    _streamState.extData.seqScalExt.vert_subsmp_fact_m = _bitBuffer.GetBits(5);
	    _streamState.extData.seqScalExt.vert_subsmp_fact_n = _bitBuffer.GetBits(5);
	}

	if (sequence_scalable_extension::temporal_scalability ==
	    _streamState.extData.seqScalExt.scalable_mode) {
	    _streamState.extData.seqScalExt.pict_mux_enable = _bitBuffer.GetBits(1);
	    if (1 == _streamState.extData.seqScalExt.pict_mux_enable) {
		_streamState.extData.seqScalExt.mux_to_prog_seq = _bitBuffer.GetBits(1);
	    }
	    _streamState.extData.seqScalExt.pict_mux_order  = _bitBuffer.GetBits(3);
	    _streamState.extData.seqScalExt.pict_mux_factor = _bitBuffer.GetBits(3);
	}

	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}

int32_t SeqExtParser::ParseQuantMatrixExt(void)
{
    int32_t  status = 0;
    uint32_t i      = 0;

    do {
	_streamState.extData.quantMtxExt.ld_intra_quant_mtx = _bitBuffer.GetBits(1);
	if (1 == _streamState.extData.quantMtxExt.ld_intra_quant_mtx) {
	    for (i = 0; i < quant_mtx_sz; i++) {
		_streamState.extData.quantMtxExt.intra_quant_mtx[i] = _bitBuffer.GetBits(8);
	    }
	}
	
	_streamState.extData.quantMtxExt.ld_non_intra_quant_mtx = _bitBuffer.GetBits(1);
	if (1 == _streamState.extData.quantMtxExt.ld_non_intra_quant_mtx) {
	    for (i = 0; i < quant_mtx_sz; i++) {
		_streamState.extData.quantMtxExt.non_intra_quant_mtx[i] = _bitBuffer.GetBits(8);
	    }
	}
	
	_streamState.extData.quantMtxExt.ld_chroma_intra_quant_mtx = _bitBuffer.GetBits(1);
	if (1 == _streamState.extData.quantMtxExt.ld_chroma_intra_quant_mtx) {
	    for (i = 0; i < quant_mtx_sz; i++) {
		_streamState.extData.quantMtxExt.chroma_intra_quant_mtx[i] = _bitBuffer.GetBits(8);
	    }
	}
	
	_streamState.extData.quantMtxExt.ld_chroma_non_intra_quant_mtx = _bitBuffer.GetBits(1);
	if (1 == _streamState.extData.quantMtxExt.ld_chroma_non_intra_quant_mtx) {
	    for (i = 0; i < quant_mtx_sz; i++) {
		_streamState.extData.quantMtxExt.chroma_non_intra_quant_mtx[i] =
		    _bitBuffer.GetBits(8);
	    }
	}

	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}

int32_t SeqExtParser::ParseCopyrightExt(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    
    do {
	_streamState.extData.cpyRightExt.copyright_flag   = _bitBuffer.GetBits(1);
	_streamState.extData.cpyRightExt.copyright_id     = _bitBuffer.GetBits(8);
	_streamState.extData.cpyRightExt.original_or_copy = _bitBuffer.GetBits(1);

	_bitBuffer.GetBits(7);  // reserved bits
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.cpyRightExt.copyright_num_1 = _bitBuffer.GetBits(20);
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.cpyRightExt.copyright_num_2 = _bitBuffer.GetBits(22);
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.cpyRightExt.copyright_num_3 = _bitBuffer.GetBits(22);
	
	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}

int32_t SeqExtParser::ParsePictureDispExt(void)
{
    int32_t  status  = 0;
    uint32_t marker  = 0;
    uint32_t num_off = 0;
    uint32_t i       = 0;

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

	if (1 == _streamState.extData.seqExt.progressive_seq) {
	    if (1 == picData->picCodingExt.repeat_first_field) {
		if (1 == picData->picCodingExt.top_field_first) {
		    num_off = 3;
		} else {
		    num_off = 2;
		}
	    } else {
		num_off = 1;
	    }
	} else {
	    if (picData->picCodingExt.picture_struct == PictureCodingExtension::PIC_STRUCT_TOP_FIELD ||
		picData->picCodingExt.picture_struct == PictureCodingExtension::PIC_STRUCT_BOT_FIELD) {
		num_off = 1;
	    } else {
		if (1 == picData->picCodingExt.repeat_first_field) {
		    num_off = 3;
		} else {
		    num_off = 2;
		}
	    }
	}

	for (i = 0; i < num_off; i++) {
	    _streamState.extData.pictDispExt.data[i].frm_center_horz_off = _bitBuffer.GetBits(16);
	    marker = _bitBuffer.GetBits(1);
	    CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	    _streamState.extData.pictDispExt.data[i].frm_center_vert_off = _bitBuffer.GetBits(16);
	    marker = _bitBuffer.GetBits(1);
	    CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	}
	
	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}

int32_t SeqExtParser::ParsePictSpatialScalExt(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    
    do {
	_streamState.extData.pictSpatScalExt.lwr_lyr_temporal_ref = _bitBuffer.GetBits(10);
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.pictSpatScalExt.lwr_lyr_horz_off = _bitBuffer.GetBits(15);
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.pictSpatScalExt.lwr_lyr_vert_off        = _bitBuffer.GetBits(15);
	_streamState.extData.pictSpatScalExt.spat_temp_wt_cd_tbl_idx = _bitBuffer.GetBits(2);
	_streamState.extData.pictSpatScalExt.lwr_lyr_prog_frm        = _bitBuffer.GetBits(1);
	_streamState.extData.pictSpatScalExt.lwr_lyr_deint_fld_sel   = _bitBuffer.GetBits(1);

	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}

int32_t SeqExtParser::ParsePictTemporalScalExt(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;

    do {
	_streamState.extData.pictTempScalExt.ref_sel_code      = _bitBuffer.GetBits(2);
	_streamState.extData.pictTempScalExt.forw_temporal_ref = _bitBuffer.GetBits(10);
	marker = _bitBuffer.GetBits(1);
	CHECK_VALUE_AND_BREAK(1, marker, status, -1);
	_streamState.extData.pictTempScalExt.back_temporal_ref = _bitBuffer.GetBits(10);

	_bitBuffer.GetNextStartCode();
    } while (0);

    return status;
}
