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
    f_code_forw_horz(0),
    f_code_forw_vert(0),
    f_code_back_horz(0),
    f_code_back_vert(0),
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
}

void PictureCodingExtension::ResetData(void)
{
    f_code_forw_horz     = 0;
    f_code_forw_vert     = 0;
    f_code_back_horz     = 0;
    f_code_back_vert     = 0;
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
    spatial_temporal_weight_code_flag(0)
{
}

void MacroblkData::ResetData(void)
{
    macroblock_address_inc            = 0;
    quantiser_scale_code              = 0;
    macroblock_quant                  = 0;
    macroblock_motion_forw            = 0;
    macroblock_motion_back            = 0;
    macroblock_pattern                = 0;
    macroblock_intra                  = 0;
    spatial_temporal_weight_code_flag = 0;
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
