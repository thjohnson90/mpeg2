#ifndef __BASE_PARSER_H__
#define __BASE_PARSER_H__

class FileBitBuffer;
class BufBitBuffer;

class BaseParser
{
public:
    BaseParser(FileBitBuffer& fbb, BufBitBuffer& bbb, StreamState& ss);
    ~BaseParser();
    
    uint32_t Initialize(void);
    uint32_t Destroy(void);
    uint32_t ParseVideoSequence(void);
    uint32_t ParseMPEG2Stream(void);
    uint32_t ParseExtensionUserData(uint32_t flag);
    uint32_t Ring(uint32_t cmd) {_cmd = cmd; return _bell.Ring();}
    uint32_t WorkerListen(void) {return _worker.Listen();}
    uint32_t GetWorkerCmd(void) {return _worker.GetCmd();}
    
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
    
private:
    static void* ParserWorker(void*);
    
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
    Doorbell          _bell;
    Thread            _worker;
    uint32_t          _cmd;
};

#endif
