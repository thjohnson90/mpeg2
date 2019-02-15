#ifndef __PICTURE_H__
#define __PICTURE_H__

class PictureParser
{
public:
    PictureParser(BitBuffer& bb, StreamState& ss);
    ~PictureParser();

    uint32_t Initialize(void);
    uint32_t Destroy(void);
    
    uint32_t ParsePictureHdr(void);
    uint32_t ParsePictCodingExt(void);
    uint32_t ParsePictData(void);

    enum {
	SEQ_EXT_ID            = 1,
	SEQ_DISP_EXT_ID       = 2,
	QUANT_MTX_EXT_ID      = 3,
	COPYRIGHT_EXT_ID      = 4,
	SEQ_SCAL_EXT_ID       = 5,
	PICT_DISP_EXT_ID      = 7,
	PICT_CODING_EXT_ID    = 8,
	PICT_SPAT_SCAL_EXT_ID = 9,
	PICT_TEMP_SCAL_EXT_IT = 10
    };
    
private:
    SliceParser* GetSliceParser(void);
    
    SliceParser* _sliceParser;
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif
