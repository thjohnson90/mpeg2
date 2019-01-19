#ifndef __EXT_HDR_H__
#define __EXT_HDR_H__

class SeqExtParser
{
public:
    SeqExtParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParseSequenceExt(void);
    uint32_t ParseExtensionData(uint32_t flag);

    enum {
	seq_ext                    = 1,
	seq_display_ext            = 2,
	quant_matrix_ext           = 3,
	copyright_ext              = 4,
	seq_scalable_ext           = 5,
	pict_display_ext           = 7,
	pict_coding_ext            = 8,
	pict_spatial_scalable_ext  = 9,
	pict_temporal_scalable_ext = 10,
	ext_start_code_id_size     = 4
    };
    
protected:
    uint32_t ParseSeqDisplayExt(void);
    uint32_t ParseSeqScalableExt(void);
    uint32_t ParseQuantMatrixExt(void);
    uint32_t ParseCopyrightExt(void);
    uint32_t ParsePictureDispExt(void);
    uint32_t ParsePictSpatialScalExt(void);
    uint32_t ParsePictTemporalScalExt(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
