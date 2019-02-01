#ifndef __BASE_PARSER_H__
#define __BASE_PARSER_H__

class BaseParser
{
public:
    BaseParser(BitBuffer& bb, StreamState& ss);
    ~BaseParser();
    
    uint32_t Initialize(void);
    uint32_t Destroy(void);
    uint32_t ParseVideoSequence(void);
    uint32_t ParseMPEG2Stream(void);
    uint32_t ParseExtensionUserData(uint32_t flag);
    
protected:
    PackHdrParser*    GetPackHdrParser(void);
    SystemHdrParser*  GetSystemHdrParser(void);
    PesHeaderParser*  GetPesHdrParser(void);
    SeqHdrParser*     GetSeqHdrParser(void);
    SeqExtParser*     GetSeqExtParser(void);
    UserDataParser*   GetUserDataParser(void);
    GopHdrParser*     GetGopHdrParser(void);
    PictureParser*    GetPictureParser(void);
    SliceParser*      GetSliceParser(void);
    
private:
    BitBuffer&        _bitBuffer;
    StreamState&      _streamState;
    PackHdrParser*    _packHdrParser;
    SystemHdrParser*  _systemHdrParser;
    PesHeaderParser*  _pesHdrParser;
    SeqHdrParser*     _seqHdrParser;
    SeqExtParser*     _seqExtParser;
    UserDataParser*   _userDataParser;
    GopHdrParser*     _gopParser;
    PictureParser*    _picParser;
    SliceParser*      _sliceParser;
};

#endif
