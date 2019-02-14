#ifndef __BASE_PARSER_H__
#define __BASE_PARSER_H__

class FileBitBuffer;
class BufBitBuffer;

class BaseParser
{
public:
    BaseParser(FileBitBuffer& fbb, BufBitBuffer& bbb, StreamState& ss);
    ~BaseParser();
    
    enum {
	parse_cmd_null,
	parse_cmd_data_ready,
	parse_cmd_data_consumed,
	parse_cmd_seq_end_received,
	parse_cmd_exit
    };
    
    uint32_t Initialize(void);
    uint32_t Destroy(void);
    uint32_t ParseVideoSequence(void);
    uint32_t ParseMPEG2Stream(void);
    uint32_t ParseExtensionUserData(uint32_t flag);
    uint32_t SendMainMessage(uint32_t cmd);
    uint32_t WaitMainMessage(uint32_t& cmd);
    uint32_t SendWorkMessage(uint32_t cmd);
    uint32_t WaitWorkMessage(uint32_t& cmd);
    
protected:
    PackHdrParser*    GetPackHdrParser(void);
    SystemHdrParser*  GetSystemHdrParser(void);
    PesHeaderParser*  GetPesHdrParser(void);
    PesPacketParser*  GetPesPacketParser(void);
    SeqHdrParser*     GetSeqHdrParser(void);
    SeqExtParser*     GetSeqExtParser(void);
    UserDataParser*   GetUserDataParser(void);
    GopHdrParser*     GetGopHdrParser(void);
    PictureParser*    GetPictureParser(void);
    SliceParser*      GetSliceParser(void);
    
private:
    FileBitBuffer&    _fbitBuffer;
    BufBitBuffer&     _bbitBuffer;
    StreamState&      _streamState;
    PackHdrParser*    _packHdrParser;
    SystemHdrParser*  _systemHdrParser;
    PesHeaderParser*  _pesHdrParser;
    PesPacketParser*  _pesPacketParser;
    SeqHdrParser*     _seqHdrParser;
    SeqExtParser*     _seqExtParser;
    UserDataParser*   _userDataParser;
    GopHdrParser*     _gopParser;
    PictureParser*    _picParser;
    SliceParser*      _sliceParser;
    bool              _parseThrdActive;
    pthread_t         _parseThrdId;
    pthread_mutex_t   _mainDoorbellMutex;
    pthread_cond_t    _mainDoorbellCond;
    pthread_mutex_t   _workDoorbellMutex;
    pthread_cond_t    _workDoorbellCond;
    uint32_t          _parseCmd;

    static void* ParserWorker(void* pThis);
    void DumpCommand(uint32_t cmd);
};

#endif
