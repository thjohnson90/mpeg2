#ifndef __STREAM_H__
#define __STREAM_H__

#define CHECK_PARSE(func,stat) {if (0 > ((stat) = (func))) {break;}}

struct pack_header
{
    pack_header();
    
    uint32_t SCR32_30        : 3;
    uint32_t SCR29_15        : 15;
    uint32_t SCR14_00        : 15;
    uint32_t SCR_EXT         : 9;
    uint32_t ProgramMuxRate  : 22;
    uint32_t Reserved        : 5;
    uint32_t PackStuffingLen : 3;
};

struct stream_id_syshdr {
    stream_id_syshdr();

    uint32_t stream_id                    : 8;
    uint32_t PSTD_buffer_bound_scale      : 1;
    uint32_t PSTD_buffer_size_bound       : 13;
    struct stream_id_syshdr* pNext;
};
    
struct system_header
{
    system_header();
    ~system_header();
    void reset_system_header(void);
    
    uint32_t hdr_len                      : 16;
    uint32_t rate_bound	                  : 22;
    uint32_t audio_bound                  : 6;
    uint32_t fixed_flag                   : 1;
    uint32_t CSPS_flag                    : 1;
    uint32_t system_audio_lock_flag       : 1;
    uint32_t system_video_lock_flag       : 1;
    uint32_t video_bound                  : 5;
    uint32_t packet_rate_restriction_flag : 1;
    uint32_t reserved_byte                : 7;
    struct stream_id_syshdr* pNextStrmId;
};

struct pes_header_ext
{
    pes_header_ext();
    
    uint32_t PES_scrambling_control   : 2;
    uint32_t PES_priority             : 1;
    uint32_t data_alignment_indicator : 1;
    uint32_t copyright                : 1;
    uint32_t original_copy            : 1;
    uint32_t PTS_DTS_flags            : 2;
    uint32_t ESCR_flag                : 1;
    uint32_t ES_rate_flag             : 1;
    uint32_t DSM_trick_mode_flag      : 1;
    uint32_t addition_copy_info_flag  : 1;
    uint32_t PES_CRC_flag             : 1;
    uint32_t PES_extension_flag       : 1;
    uint32_t PES_header_data_len      : 8;
    uint32_t PTS32_30                 : 3;
    uint32_t PTS29_15                 : 15;
    uint32_t PTS14_00                 : 15;
    uint32_t DTS32_30                 : 3;
    uint32_t DTS29_15                 : 15;
    uint32_t DTS14_00                 : 15;
    uint32_t ESCR32_30                : 3;
    uint32_t ESCR29_15                : 15;
    uint32_t ESCR14_00                : 15;
    uint32_t ESCR_ext                 : 9;
    uint32_t ES_rate                  : 22;
    uint32_t additional_copy_info     : 7;
    uint32_t previous_PES_packet_CRC  : 16;
    uint32_t PES_private_data_flag    : 1;
    uint32_t pack_header_field_flag   : 1;
    uint32_t prog_packet_seq_ctr_flag : 1;
    uint32_t PSTD_buff_flag           : 1;
    uint32_t PES_ext_flag2            : 1;
    uint32_t packet_seq_counter       : 7;
    uint32_t MPEG1_MPEG2_identifier   : 1;
    uint32_t original_stuffing_len    : 6;
    uint32_t PSTD_buffer_scale        : 1;
    uint32_t PSTD_buffer_size         : 13;
    uint32_t PES_ext_field_len        : 7;
    uint32_t reserved                 : 8;
};

struct pes_header
{
    pes_header();
    
    uint32_t stream_id  : 8;
    uint32_t packet_len : 16;
    struct pes_header_ext ext;
};

struct sequence_header
{
    sequence_header();
    
    uint32_t horizontal_sz               : 12;
    uint32_t vertical_sz                 : 12;
    uint32_t aspect_ratio                : 4;
    uint32_t frame_rate                  : 4;
    uint32_t bit_rate                    : 18;
    uint32_t VBV_buffer_sz               : 10;
    uint32_t constrained_params_flag     : 1;
    uint32_t load_intra_quant_matrix     : 1;
    uint32_t load_non_intra_quant_matrix : 1;
    uint8_t  intra_quant_matrix[64];
    uint8_t  non_intra_quant_matrix[64];
};

struct sequence_extension
{
    sequence_extension();

    enum {
	CHROMA_FMT_RESERVED = 0,
	CHROMA_FMT_420      = 1,
	CHROMA_FMT_422      = 2,
	CHROMA_FMT_444      = 3
    };
    
    // sequence extension
    uint32_t profile_level        : 8;
    uint32_t progressive_seq      : 1;
    uint32_t chroma_format        : 2;
    uint32_t horz_sz_ext          : 2;
    uint32_t vert_sz_ext          : 2;
    uint32_t bit_rate_ext         : 12;
    uint32_t vbv_buf_sz_ext       : 8;
    uint32_t low_delay            : 1;
    uint32_t frame_rate_ext_n     : 2;
    uint32_t frame_rate_ext_d     : 5;
};

struct sequence_display_extension
{
    sequence_display_extension();
    
    // sequence display extension
    uint32_t video_format         : 3;
    uint32_t color_desc_flag      : 1;
    uint32_t color_primaries      : 8;
    uint32_t transfer_char        : 8;
    uint32_t matrix_coeff         : 8;
    uint32_t display_horz_sz      : 14;
    uint32_t display_vert_sz      : 14;
};

struct sequence_scalable_extension
{
    sequence_scalable_extension();
    enum {
	data_partitioning    = 0,
	spatial_scalability  = 1,
	SNR_scalability      = 2,
	temporal_scalability = 3
    };

    // sequence scalable extension
    uint32_t scalable_mode        : 2;
    uint32_t layer_id             : 4;
    uint32_t lwr_lyr_pred_horz_sz : 14;
    uint32_t lwr_lyr_pred_vert_sz : 14;
    uint32_t horz_subsmp_fact_m   : 5;
    uint32_t horz_subsmp_fact_n   : 5;
    uint32_t vert_subsmp_fact_m   : 5;
    uint32_t vert_subsmp_fact_n   : 5;
    uint32_t pict_mux_enable      : 1;
    uint32_t mux_to_prog_seq      : 1;
    uint32_t pict_mux_order       : 3;
    uint32_t pict_mux_factor      : 3;

    bool     present;
};

struct quant_matrix_extension
{
    quant_matrix_extension();
    enum{quant_mtx_sz = 64};
    
    // quantization matrix extension
    uint32_t ld_intra_quant_mtx            : 1;
    uint32_t ld_non_intra_quant_mtx        : 1;
    uint32_t ld_chroma_intra_quant_mtx     : 1;
    uint32_t ld_chroma_non_intra_quant_mtx : 1;
    uint8_t  intra_quant_mtx[quant_mtx_sz];
    uint8_t  non_intra_quant_mtx[quant_mtx_sz];
    uint8_t  chroma_intra_quant_mtx[quant_mtx_sz];
    uint8_t  chroma_non_intra_quant_mtx[quant_mtx_sz];
};

struct copyright_extension
{
    copyright_extension();

    // copyright extension
    uint32_t copyright_flag       : 1;
    uint32_t copyright_id         : 8;
    uint32_t original_or_copy     : 1;
    uint32_t copyright_num_1      : 20;
    uint32_t copyright_num_2      : 22;
    uint32_t copyright_num_3      : 22;
};

struct picture_display_extension
{
    picture_display_extension();
    enum {max_disp_ext_cnt = 3};

    // picture display extension
    struct {
	uint32_t frm_center_horz_off : 16;
	uint32_t frm_center_vert_off : 16;
    } data[max_disp_ext_cnt];
};

struct picture_spatial_scalable_extension
{
    picture_spatial_scalable_extension();

    // picture spatial scalable extension
    uint32_t lwr_lyr_temporal_ref    : 10;
    uint32_t lwr_lyr_horz_off        : 15;
    uint32_t lwr_lyr_vert_off        : 15;
    uint32_t spat_temp_wt_cd_tbl_idx : 2;
    uint32_t lwr_lyr_prog_frm        : 1;
    uint32_t lwr_lyr_deint_fld_sel   : 1;
};

struct picture_temporal_scalable_extension
{
    picture_temporal_scalable_extension();

    // picture temporal scalable extension
    uint32_t ref_sel_code            : 2;
    uint32_t forw_temporal_ref       : 10;
    uint32_t back_temporal_ref       : 10;
};

struct extension_data
{
    extension_data();

    sequence_extension                  seqExt;
    sequence_display_extension          seqDispExt;
    sequence_scalable_extension         seqScalExt;
    quant_matrix_extension              quantMtxExt;
    copyright_extension                 cpyRightExt;
    picture_display_extension           pictDispExt;
    picture_spatial_scalable_extension  pictSpatScalExt;
    picture_temporal_scalable_extension pictTempScalExt;
};

struct gop
{
    gop();
    
    uint32_t drop_frame_flag    : 1;
    uint32_t time_code_hours    : 5;
    uint32_t time_code_minutes  : 6;
    uint32_t time_code_seconds  : 6;
    uint32_t time_code_pictures : 6;
    uint32_t closed_gop         : 1;
    uint32_t broken_link        : 1;
};

struct StreamState
{
    StreamState();
    
    enum {
        picture_start        = 0,
        slice_start_min      = 0x01,
        slice_start_max      = 0xAF,
        reserved1            = 0xB0,
        reserved2            = 0xB1,
        user_data_start      = 0xB2,
        sequence_header      = 0xB3,
        sequence_error       = 0xB4,
        extension_start      = 0xB5,
        reserved3            = 0xB6,
        sequence_end         = 0xB7,
        group_start          = 0xB8,
        system_start_min     = 0xB9,
	program_end          = 0xB9,
        pack_header          = 0xBA,
        system_header        = 0xBB,
        program_stream_map   = 0xBC,
        pes_private_stream1  = 0xBD,
        pes_padding_stream   = 0xBE,
        pes_private_stream2  = 0xBF,
        pes_audio_stream_min = 0xC0,
        pes_audio_stream_max = 0xDF,
        pes_video_stream_min = 0xE0,
        pes_video_stream_max = 0xEF,
        ECM_stream           = 0xF0,
        EMM_stream           = 0xF1,
        DSMCC_stream         = 0xF2,
        IEC_13522_stream     = 0xF3,
        H222_1_type_A_stream = 0xF4,
        H222_1_type_B_stream = 0xF5,
        H222_1_type_C_stream = 0xF6,
        H222_1_type_D_stream = 0xF7,
        H222_1_type_E_stream = 0xF8,
        ancillary_stream     = 0xF9,
        program_stream_dir   = 0xFF,
        system_start_max     = 0xFF
    } START_CODES;
    
    struct pack_header        packHdr;
    struct system_header      sysHdr;
    struct pes_header         pesHdr;
    struct sequence_header    seqHdr;
    struct extension_data     extData;
    struct gop                gop;
};


#define CHECK_VALUE_AND_BREAK(expected, value, status, err)     \
    if ((expected) != (value)) {status = err; break; }

#endif
