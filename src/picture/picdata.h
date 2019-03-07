#ifndef __PICDATA_H__
#define __PICDATA_H__

struct PictureHeader
{
    PictureHeader();
    void ResetData(void);
    
    enum {
        PIC_CODING_TYPE_I = 1,
        PIC_CODING_TYPE_P = 2,
        PIC_CODING_TYPE_B = 3,
        PIC_CODING_TYPE_D = 4
    };

    uint32_t temporal_reference   : 10;
    uint32_t picture_coding_type  : 3;
    uint32_t vbv_delay            : 16;
    uint32_t full_pel_forw_vector : 1;
    uint32_t forw_f_code          : 3;
    uint32_t full_pel_back_vector : 1;
    uint32_t back_f_code          : 3;
    uint32_t extra_bit_pict       : 1;
    uint32_t extra_info_pict      : 8;
};

struct PictureCodingExtension
{
    PictureCodingExtension();
    void ResetData(void);
    
    enum {
	PIC_STRUCT_TOP_FIELD = 1,
	PIC_STRUCT_BOT_FIELD = 2,
	PIC_STRUCT_FRAME     = 3
    };

    uint32_t f_code[2][2];
    
    uint32_t intra_dc_prec        : 2;
    uint32_t picture_struct       : 2;
    uint32_t top_field_first      : 1;
    uint32_t frame_pred_frame_dct : 1;
    uint32_t concealment_mot_vecs : 1;
    uint32_t q_scale_type         : 1;
    uint32_t intra_vlc_format     : 1;
    uint32_t alternate_scan       : 1;
    uint32_t repeat_first_field   : 1;
    uint32_t chroma_420_type      : 1;
    uint32_t progressive_frame    : 1;
    uint32_t composite_display    : 1;
    uint32_t v_axis               : 1;
    uint32_t field_sequence       : 3;
    uint32_t sub_carrier          : 1;
    uint32_t burst_amplitude      : 7;
    uint32_t sub_carrier_phase    : 8;
};

struct SliceData
{
    SliceData();
    void ResetData(void);
    
    uint32_t slice_vertical_position_ext : 3;
    uint32_t priority_breakpoint         : 7;
    uint32_t quantizer_scale_code        : 5;
    uint32_t intra_slice_flag            : 1;
    uint32_t intra_slice                 : 1;
    uint32_t extra_bit_slice             : 1;
    uint32_t extra_information_slice     : 8;
};

struct MacroblkData
{
    MacroblkData();
    void ResetData(void);

    enum {
	MVFMT_FIELD = 0,
	MVFMT_FRAME = 1
    };

    uint32_t macroblock_address_inc            : 6;
    uint32_t quantiser_scale_code              : 5;
    uint32_t macroblock_quant                  : 1;
    uint32_t macroblock_motion_forw            : 1;
    uint32_t macroblock_motion_back            : 1;
    uint32_t macroblock_pattern                : 1;
    uint32_t macroblock_intra                  : 1;
    uint32_t spatial_temporal_weight_code_flag : 1;
    uint32_t spatial_temporal_weight_code      : 2;
    uint32_t frame_motion_type                 : 2;
    uint32_t field_motion_type                 : 2;
    uint32_t dct_type                          : 1;
    uint32_t spatial_temporal_weight_class     : 3;
    uint32_t spatial_temporal_integer_weight   : 1;
    uint32_t motion_vector_count               : 2;
    uint32_t mv_format                         : 1;
    uint32_t dmv                               : 1;
    
    uint32_t block_count                       : 4;

    uint8_t motion_vertical_field_select[2][2];
    uint32_t coded_block_pattern_420;
    uint32_t coded_block_pattern_1;
    uint32_t coded_block_pattern_2;
};

struct MotionVecData {
    MotionVecData();
    void ResetData(void);

    int32_t  motion_code[2][2][2];
    uint32_t motion_residual[2][2][2];
    int32_t  dmvector[2];
};

struct BlockData {
    BlockData();
    void ResetData(void);
    
    uint32_t pattern_code[12];
    int32_t  dct_dc_pred[3];
    int32_t  coeff[3];
    
    uint32_t dct_dc_size_luminance     : 9;
    uint32_t dct_dc_size_chrominance   : 10;

    int32_t dct_dc_differential_lum    : 11;
    int32_t dct_dc_differential_chrom  : 11;

    int32_t QFS[64];
};

struct PictureData
{
    PictureData();
    void NullPictureData(void);
    void ResetDctDcPred(void);
    
    struct PictureHeader          picHdr;
    struct PictureCodingExtension picCodingExt;
    struct SliceData              sliceData;
    struct MacroblkData           macroblkData;
    struct MotionVecData          mvData;
    struct BlockData              blkData;
};

class PictureDataMgr
{
public:
    PictureDataMgr(StreamState& ss);
    ~PictureDataMgr();
    
    static PictureDataMgr* GetPictureDataMgr(StreamState& ss);
    void                   ReleasePictureDataMgr(void);
    PictureData*           GetNextBuffer(void);
    PictureData*           GetCurrentBuffer(void);
    PictureData*           GetBackBuffer(void) {return _backBuf;}
    PictureData*           GetFrontBuffer(void) {return _frontBuf;}
    
private:
    PictureDataMgr(const PictureDataMgr&);
    PictureDataMgr& operator=(const PictureDataMgr&);
    
    static PictureDataMgr* _PictureDataMgr;
    
    PictureData*           _frontBuf;
    PictureData*           _backBuf;
    StreamState&           _strmState;
};

#endif

