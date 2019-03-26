#include <stdint.h>
#include <string.h>

#include "stream.h"


pack_header::pack_header() :
    SCR32_30(0),
    SCR29_15(0),
    SCR14_00(0),
    SCR_EXT(0),
    ProgramMuxRate(0),
    Reserved(0),
    PackStuffingLen(0)
{
}

stream_id_syshdr::stream_id_syshdr() :
    stream_id(0),
    PSTD_buffer_bound_scale(0),
    PSTD_buffer_size_bound(0),
    pNext(0)
{
}

system_header::system_header() :
    hdr_len(0),
    rate_bound(0),
    audio_bound(0),
    fixed_flag(0),
    CSPS_flag(0),
    system_audio_lock_flag(0),
    system_video_lock_flag(0),
    video_bound(0),
    packet_rate_restriction_flag(0),
    reserved_byte(0),
    pNextStrmId(0)
{
}

system_header::~system_header()
{
    reset_system_header();
}

void system_header::reset_system_header(void)
{
    hdr_len                      = 0;
    rate_bound                   = 0;
    audio_bound                  = 0;
    fixed_flag                   = 0;
    CSPS_flag                    = 0;
    system_audio_lock_flag       = 0;
    system_video_lock_flag       = 0;
    video_bound                  = 0;
    packet_rate_restriction_flag = 0;
    reserved_byte                = 0;

    // release memory allocated for each stream ID
    struct stream_id_syshdr* pStrmId = pNextStrmId;

    while (0 != pStrmId) {
        struct stream_id_syshdr* pTmp = pStrmId->pNext;
        delete pStrmId;
        pStrmId = pTmp;
    }

    pNextStrmId = 0;
}

pes_header_ext::pes_header_ext() : 
    PES_scrambling_control(0),
    PES_priority(0),
    data_alignment_indicator(0),
    copyright(0),
    original_copy(0),
    PTS_DTS_flags(0),
    ESCR_flag(0),
    ES_rate_flag(0),
    DSM_trick_mode_flag(0),
    addition_copy_info_flag(0),
    PES_CRC_flag(0),
    PES_extension_flag(0),
    PES_header_data_len(0),
    PTS32_30(0),
    PTS29_15(0),
    PTS14_00(0),
    DTS32_30(0),
    DTS29_15(0),
    DTS14_00(0),
    ESCR32_30(0),
    ESCR29_15(0),
    ESCR14_00(0),
    ESCR_ext(0),
    ES_rate(0),
    additional_copy_info(0),
    previous_PES_packet_CRC(0),
    PES_private_data_flag(0),
    pack_header_field_flag(0),
    prog_packet_seq_ctr_flag(0),
    PSTD_buff_flag(0),
    PES_ext_flag2(0),
    packet_seq_counter(0),
    MPEG1_MPEG2_identifier(0),
    original_stuffing_len(0),
    PSTD_buffer_scale(0),
    PSTD_buffer_size(0),
    PES_ext_field_len(0),
    reserved(0)
{
}

pes_header::pes_header() :
    stream_id(0),
    packet_len(0)
{
}

sequence_header::sequence_header() :
    horizontal_sz(0),
    vertical_sz(0),
    aspect_ratio(0),
    frame_rate(0),
    bit_rate(0),
    VBV_buffer_sz(0),
    constrained_params_flag(0),
    load_intra_quant_matrix(0),
    load_non_intra_quant_matrix(0)
{
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 8; j++) {
	    for (int k = 0; k < 8; k++) {
		W[i][j][k] = 0;
	    }
	}
    }
}

sequence_extension::sequence_extension() :
    profile_level(0),
    progressive_seq(0),
    chroma_format(0),
    horz_sz_ext(0),
    vert_sz_ext(0),
    bit_rate_ext(0),
    vbv_buf_sz_ext(0),
    low_delay(0),
    frame_rate_ext_n(0),
    frame_rate_ext_d(0)
{
}

sequence_display_extension::sequence_display_extension() :
    video_format(0),
    color_desc_flag(0),
    color_primaries(0),
    transfer_char(0),
    matrix_coeff(0),
    display_horz_sz(0),
    display_vert_sz(0)
{
}

sequence_scalable_extension::sequence_scalable_extension() :
    scalable_mode(0),
    layer_id(0),
    lwr_lyr_pred_horz_sz(0),
    lwr_lyr_pred_vert_sz(0),
    horz_subsmp_fact_m(0),
    horz_subsmp_fact_n(0),
    vert_subsmp_fact_m(0),
    vert_subsmp_fact_n(0),
    pict_mux_enable(0),
    mux_to_prog_seq(0),
    pict_mux_order(0),
    pict_mux_factor(0),
    present(false)
{
}

quant_matrix_extension::quant_matrix_extension() :
    ld_intra_quant_mtx(0),
    ld_non_intra_quant_mtx(0),
    ld_chroma_intra_quant_mtx(0),
    ld_chroma_non_intra_quant_mtx(0)
{
    memset(intra_quant_mtx, 0, sizeof(intra_quant_mtx));
    memset(non_intra_quant_mtx, 0, sizeof(non_intra_quant_mtx));
    memset(chroma_intra_quant_mtx, 0, sizeof(chroma_intra_quant_mtx));
    memset(chroma_non_intra_quant_mtx, 0, sizeof(chroma_non_intra_quant_mtx));
}

copyright_extension::copyright_extension() :
    copyright_flag(0),
    copyright_id(0),
    original_or_copy(0),
    copyright_num_1(0),
    copyright_num_2(0),
    copyright_num_3(0)
{
}

picture_display_extension::picture_display_extension()
{
    for (int i = 0; i < max_disp_ext_cnt; i++) {
	data[i].frm_center_horz_off = 0;
	data[i].frm_center_vert_off = 0;
    }
}

picture_spatial_scalable_extension::picture_spatial_scalable_extension() :
    lwr_lyr_temporal_ref(0),
    lwr_lyr_horz_off(0),
    lwr_lyr_vert_off(0),
    spat_temp_wt_cd_tbl_idx(0),
    lwr_lyr_prog_frm(0),
    lwr_lyr_deint_fld_sel(0)
{
}

picture_temporal_scalable_extension::picture_temporal_scalable_extension() :
    ref_sel_code(0),
    forw_temporal_ref(0),
    back_temporal_ref(0)
{
}

extension_data::extension_data()
{
}

gop::gop() :
    drop_frame_flag(0),
    time_code_hours(0),
    time_code_minutes(0),
    time_code_seconds(0),
    time_code_pictures(0),
    closed_gop(0),
    broken_link(0)
{
}

StreamState::StreamState()
    
{
    for (int i = 0; i < 64; i++) {
        seqHdr.intra_quant_matrix[i]     = 0;
        seqHdr.non_intra_quant_matrix[i] = 0;
    }
}
