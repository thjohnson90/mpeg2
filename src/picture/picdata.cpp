#include <stdint.h>

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"

PictureHeader::PictureHeader() :
    temporal_reference(0),
    picture_coding_type(0),
    vbv_delay(0),
    full_pel_forw_vector(0),
    forw_f_code(0),
    full_pel_back_vector(0),
    back_f_code(0)
{
}

void PictureHeader::ResetData(void)
{
    temporal_reference   = 0;
    picture_coding_type  = 0;
    vbv_delay            = 0;
    full_pel_forw_vector = 0;
    forw_f_code          = 0;
    full_pel_back_vector = 0;
    back_f_code          = 0;
}

PictureCodingExtension::PictureCodingExtension() :
    intra_dc_prec(0),
    picture_struct(0),
    top_field_first(0),
    frame_pred_frame_dct(0),
    concealment_mot_vecs(0),
    q_scale_type(0),
    intra_vlc_format(0),
    alternate_scan(0),
    repeat_first_field(0),
    chroma_420_type(0),
    progressive_frame(0),
    composite_display(0),
    v_axis(0),
    field_sequence(0),
    sub_carrier(0),
    burst_amplitude(0),
    sub_carrier_phase(0)
{
    for (int i = 0; i < 2; i++) {
	for (int j = 0; j < 2; j++) {
	    f_code[i][j] = 0;
	}
    }
}

void PictureCodingExtension::ResetData(void)
{
    for (int i = 0; i < 2; i++) {
	for (int j = 0; j < 2; j++) {
	    f_code[i][j] = 0;
	}
    }

    intra_dc_prec        = 0;
    picture_struct       = 0;
    top_field_first      = 0;
    frame_pred_frame_dct = 0;
    concealment_mot_vecs = 0;
    q_scale_type         = 0;
    intra_vlc_format     = 0;
    alternate_scan       = 0;
    repeat_first_field   = 0;
    chroma_420_type      = 0;
    progressive_frame    = 0;
    composite_display    = 0;
    v_axis               = 0;
    field_sequence       = 0;
    sub_carrier          = 0;
    burst_amplitude      = 0;
    sub_carrier_phase    = 0;
}

SliceData::SliceData() :
    slice_vertical_position_ext(0),
    priority_breakpoint(0),
    quantizer_scale_code(0),
    intra_slice_flag(0),
    intra_slice(0),
    slice_picture_id_enable(0),
    slice_picture_id(0),
    extra_bit_slice(0),
    extra_information_slice(0)
{
}

void SliceData::ResetData(void)
{
    slice_vertical_position_ext = 0;
    priority_breakpoint         = 0;
    quantizer_scale_code        = 0;
    intra_slice_flag            = 0;
    intra_slice                 = 0;
    slice_picture_id_enable     = 0;
    slice_picture_id            = 0;
    extra_bit_slice             = 0;
    extra_information_slice     = 0;
}

MacroblkData::MacroblkData() :
    macroblock_address_inc(0),
    quantiser_scale_code(0),
    macroblock_quant(0),
    macroblock_motion_forw(0),
    macroblock_motion_back(0),
    macroblock_pattern(0),
    macroblock_intra(0),
    spatial_temporal_weight_code_flag(0),
    spatial_temporal_weight_code(0),
    frame_motion_type(0),
    field_motion_type(0),
    dct_type(0),
    spatial_temporal_weight_class(0),
    spatial_temporal_integer_weight(0),
    motion_vector_count(0),
    mv_format(MVFMT_FRAME),
    dmv(0),
    block_count(0),
    coded_block_pattern_420(0),
    coded_block_pattern_1(0),
    coded_block_pattern_2(0),
    macroblock_address(0),
    previous_macroblock_address(0),
    slice_vertical_pos(0),
    mb_row(0),
    mb_col(0),
    mb_width(0)
{
    motion_vertical_field_select[0][0] = 0;
    motion_vertical_field_select[0][1] = 0;
    motion_vertical_field_select[1][0] = 0;
    motion_vertical_field_select[1][1] = 0;
}

void MacroblkData::ResetData(void)
{
    macroblock_address_inc             = 0;
    quantiser_scale_code               = 0;
    macroblock_quant                   = 0;
    macroblock_motion_forw             = 0;
    macroblock_motion_back             = 0;
    macroblock_pattern                 = 0;
    macroblock_intra                   = 0;
    spatial_temporal_weight_code_flag  = 0;
    spatial_temporal_weight_code       = 0;
    frame_motion_type                  = 0;
    field_motion_type                  = 0;
    dct_type                           = 0;
    spatial_temporal_weight_code       = 0;
    spatial_temporal_integer_weight    = 0;
    motion_vector_count                = 0;
    mv_format                          = MVFMT_FRAME;
    dmv                                = 0;
    block_count                        = 0;
    motion_vertical_field_select[0][0] = 0;
    motion_vertical_field_select[0][1] = 0;
    motion_vertical_field_select[1][0] = 0;
    motion_vertical_field_select[1][1] = 0;
    coded_block_pattern_420            = 0;
    coded_block_pattern_1              = 0;
    coded_block_pattern_2              = 0;
    macroblock_address                 = 0;
    previous_macroblock_address        = 0;
    slice_vertical_pos                 = 0;
    mb_row                             = 0;
    mb_col                             = 0;
    mb_width                           = 0;
}

MotionVecData::MotionVecData()
{
    ResetData();
}

void MotionVecData::ResetData(void)
{
    for (int i = 0; i < 2; i++) {
	for (int j = 0; j < 2; j++) {
	    for (int k = 0; k < 2; k++) {
		motion_code[i][j][k]     = 0;
		motion_residual[i][j][k] = 0;
	    }
	}
	dmvector[i] = 0;
    }
}

BlockData::BlockData() :
    dct_dc_size_luminance(0),
    dct_dc_differential_lum(0),
    dct_dc_size_chrominance(0),
    dct_dc_differential_chrom(0)
{
    ResetData();
}

void BlockData::ResetData(void)
{
    int32_t i = 0;
    
    for (i = 0; i < 12; i++) {
	pattern_code[i] = 0;
    }

    for (i = 0; i < 3; i++) {
	dct_dc_pred[i] = 0;
	coeff[i]       = 0;
    }
    
    for (i = 0; i < 64; i++) {
	QFS[i] = 0;
    }
    
    dct_dc_size_luminance     = 0;
    dct_dc_differential_lum   = 0;
    dct_dc_size_chrominance   = 0;
    dct_dc_differential_chrom = 0;
}

PictureDataMgr* PictureDataMgr::_PictureDataMgr = 0;

PictureData::PictureData()
{
}

void PictureData::NullPictureData(void)
{
    picHdr.ResetData();
    picCodingExt.ResetData();
    sliceData.ResetData();
    macroblkData.ResetData();
}

void PictureData::ResetDctDcPred(void)
{
    int32_t i = 0;
    int32_t r = 0;
    
    switch (picCodingExt.intra_dc_prec) {
    case 0:
	r = 128;
	break;
    case 1:
	r = 256;
	break;
    case 2:
	r = 512;
	break;
    case 3:
	r = 1024;
	break;
    }

    for (i = 0; i < 3; i++) {
	blkData.dct_dc_pred[i] = r;
    }
}

PictureDataMgr::PictureDataMgr(StreamState& ss) : _frontBuf(0), _backBuf(0), _strmState(ss)
{
}

PictureDataMgr::~PictureDataMgr()
{
    delete _frontBuf; _frontBuf = 0;
    delete _backBuf;  _backBuf  = 0;
}


PictureDataMgr* PictureDataMgr::GetPictureDataMgr(StreamState& ss)
{
    if (0 == _PictureDataMgr) {
        _PictureDataMgr = new PictureDataMgr(ss);
    }
    
    return _PictureDataMgr;
}

void PictureDataMgr::ReleasePictureDataMgr(void)
{
    delete this;
    _PictureDataMgr = 0;
}

PictureData* PictureDataMgr::GetNextBuffer(void)
{
    PictureData* pictureData = 0;
    
    if (0 == _backBuf) {
        _backBuf = new PictureData;
    }
    
    if (0 == _frontBuf) {
        _frontBuf = new PictureData;
    }
    
    if (0 != _backBuf && 0 != _frontBuf) {
        PictureData* tmp = _frontBuf;
        _frontBuf = _backBuf;
        _backBuf = tmp;
        pictureData = _backBuf;
        pictureData->NullPictureData();
    }
    
    return pictureData;
}

PictureData* PictureDataMgr::GetCurrentBuffer(void)
{
    PictureData* pictureData = 0;

    if (0 == _backBuf) {
	pictureData = GetNextBuffer();
    } else {
	pictureData = _backBuf;
    }

    return pictureData;
}
