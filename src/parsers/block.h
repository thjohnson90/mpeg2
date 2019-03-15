#ifndef __BLOCK_H__
#define __BLOCK_H__

class BlockParser
{
public:
    BlockParser(BitBuffer& bb, StreamState& ss);

    int32_t Initialize(void);
    int32_t Destroy(void);

    int32_t CodedBlkPattern(PictureData* picData);
    int32_t ParseBlock(PictureData* picData, uint32_t blkcnt);
#ifdef TEST
    int32_t RunGetB15Coeff(uint32_t& run, int32_t& level, bool& eob, bool& esc)
    {return GetB15Coeff(run, level, eob, esc);}
    int32_t RunGetB14Coeff(uint32_t& run, int32_t& level, bool& eob, bool& esc, bool first = false)
    {return GetB14Coeff(run, level, eob, esc, first);}
#endif

private:
    int32_t GetPatternCode(PictureData* picData);
    int32_t GetDctSizeLuminance(PictureData* picData);
    int32_t GetDctSizeChromiance(PictureData* picData);
    int32_t ParseDctCoeff(PictureData* picData, uint32_t& run, int32_t& signed_level, bool& eob, bool first);
    int32_t GetDctDiff(PictureData* picData, uint32_t dct_dc_size, int32_t dct_dc_differential);
    int32_t GetB14Coeff(uint32_t& run, int32_t& level, bool& eob, bool& esc, bool first = false);
    int32_t GetB15Coeff(uint32_t& run, int32_t& level, bool& eob, bool& esc);
    int32_t DecodeEscCoeff(uint32_t run, int32_t signed_level);
    int32_t FillQfsArray(PictureData* picData, uint32_t run, int32_t signed_level, uint32_t& n);
    
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#define GET_COEFF(cmp, shft, max, r, l)	          \
    mask = (1 << ((max) - (shft))) - 2;	          \
    if ((cmp) == ((bits >> (shft)) & mask)) {	  \
      bits = _bitBuffer.GetBits((max) - (shft));  \
      run = (r);                                  \
      level = (l);                                \
      signed_level = static_cast<int32_t>(level); \
      if (1 == (bits & 1)) {                      \
        signed_level = -signed_level;             \
      }                                           \
      break;                                      \
    }

#endif
