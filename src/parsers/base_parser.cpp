#include <iostream>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "pack.h"
#include "syshdr.h"
#include "peshdr.h"
#include "sequence.h"
#include "extension.h"
#include "user.h"
#include "gop.h"
#include "picture.h"
#include "slice.h"
#include "base_parser.h"
#include "file_bitbuf.h"
#include "buf_bitbuf.h"

BaseParser::BaseParser(FileBitBuffer& fbb, BufBitBuffer&bbb, StreamState& ss) :
    _fbitBuffer(fbb),
    _bbitBuffer(bbb),
    _streamState(ss),
    _packHdrParser(nullptr),
    _systemHdrParser(nullptr),
    _pesHdrParser(nullptr),
    _pesPacketParser(nullptr),
    _seqHdrParser(nullptr),
    _seqExtParser(nullptr),
    _userDataParser(nullptr),
    _gopParser(nullptr),
    _picParser(nullptr),
    _sliceParser(nullptr),
    _parseThrdId(0),
    _parseCmd(parse_cmd_null)
{
    _mainDoorbellMutex = PTHREAD_MUTEX_INITIALIZER;
    _mainDoorbellCond  = PTHREAD_COND_INITIALIZER;
    _workDoorbellMutex = PTHREAD_MUTEX_INITIALIZER;
    _workDoorbellCond  = PTHREAD_COND_INITIALIZER;
}

BaseParser::~BaseParser()
{
    Destroy();
}

void* BaseParser::ParserWorker(void* arg)
{
    uint32_t cv = parse_cmd_null;
    BaseParser& parser = *static_cast<BaseParser*>(arg);
    
    pthread_mutex_lock(&parser._workDoorbellMutex);

    parser.WaitWorkMessage(cv);
    assert(parse_cmd_data_ready == cv);
    parser.ParseMPEG2Stream();

    parser.SendMainMessage(parse_cmd_seq_end_received);

    pthread_mutex_unlock(&parser._workDoorbellMutex);

    return arg;
}

void BaseParser::DumpCommand(uint32_t cmd)
{
    switch (cmd) {
    case parse_cmd_null:
	cout << "Got null cmd..." << endl;
	break;
	
    case parse_cmd_data_ready:
	cout << "Got data ready cmd..." << endl;
	break;
	
    case parse_cmd_data_consumed:
	cout << "Got data consumed cmd..." << endl;
	break;
	
    case parse_cmd_exit:
	cout << "Got exit cmd..." << endl;
	break;

    case parse_cmd_seq_end_received:
	cout << "Got sequence end cmd..." << endl;
	break;
	
    default:
	cout << "Got invalid cmd..." << endl;
	break;
    }
}

uint32_t BaseParser::Initialize(void)
{
    uint32_t status = 0;
    
    do {
	// create parsers
	if (nullptr == _packHdrParser) {
	    try {
		_packHdrParser   = GetPackHdrParser();
		_systemHdrParser = GetSystemHdrParser();
		_pesHdrParser    = GetPesHdrParser();
		_pesPacketParser = GetPesPacketParser();
		_seqHdrParser    = GetSeqHdrParser();
		_seqExtParser    = GetSeqExtParser();
		_userDataParser  = GetUserDataParser();
		_gopParser       = GetGopHdrParser();
		_picParser       = GetPictureParser();
		_sliceParser     = GetSliceParser();
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}
	
	// initalize low-level parser thread
	if (!_parseThrdActive) {
	    status = pthread_create(&_parseThrdId, 0, ParserWorker, this);
	    if (0 != status) {
		Destroy();
		status = -1;
		break;
	    }
	}
	pthread_mutex_lock(&_mainDoorbellMutex);
    } while (0);
    
    return status;
}

uint32_t BaseParser::Destroy(void)
{
    // destroy parsers
    delete _packHdrParser;   _packHdrParser   = nullptr;
    delete _systemHdrParser; _systemHdrParser = nullptr;
    delete _pesHdrParser;    _pesHdrParser    = nullptr;
    delete _pesPacketParser; _pesPacketParser = nullptr;
    delete _seqHdrParser;    _seqHdrParser    = nullptr;
    delete _seqExtParser;    _seqExtParser    = nullptr;
    delete _userDataParser;  _userDataParser  = nullptr;
    delete _gopParser;       _gopParser       = nullptr;
    delete _picParser;       _picParser       = nullptr;
    delete _sliceParser;     _sliceParser     = nullptr;

    // destroy low-level parser thread
#if 0
    pthread_mutex_lock(&_parseMutex);
    _parseCmd = parse_cmd_exit;
    pthread_cond_signal(&_parseCond);
    pthread_mutex_unlock(&_parseMutex);
#endif
    pthread_mutex_unlock(&_mainDoorbellMutex);
    pthread_join(_parseThrdId, nullptr);
}

uint32_t BaseParser::ParseVideoSequence(void)
{
    uint32_t status = 0;

    do
    {
	uint8_t cmd = _fbitBuffer.GetNextStartCode();
        // remap cmd byte to simplify switch statement
        if (cmd >= StreamState::pes_audio_stream_min &&
            cmd <= StreamState::pes_audio_stream_max) {
            // pes audio stream
            cmd &= StreamState::pes_audio_stream_min;
        } else if (cmd >= StreamState::pes_video_stream_min &&
                   cmd <= StreamState::pes_video_stream_max) {
            cmd &= StreamState::pes_video_stream_min;
        }

        switch (cmd)
        {
        case StreamState::pack_header:
            CHECK_PARSE(_packHdrParser->ParsePackHdr(), status);
            break;

        case StreamState::system_header:
            CHECK_PARSE(_systemHdrParser->ParseSystemHdr(), status);
            break;

	case StreamState::pes_audio_stream_min:
	case StreamState::pes_video_stream_min:
            CHECK_PARSE(_pesHdrParser->ParsePesHdr(), status);
	    if (StreamState::pes_video_stream_min == cmd) {
		uint32_t cv = parse_cmd_null;
		
		cout << "PES Packet Size: 0x" << hex << _streamState.pesHdr.packet_len << endl;
		uint8_t* pbuf = _bbitBuffer.GetEmptyBuffer(_streamState.pesHdr.packet_len);
		if (nullptr != pbuf) {
		    _fbitBuffer.GetBytes(pbuf, _streamState.pesHdr.packet_len);
		    CHECK_PARSE(_pesPacketParser->ChoosePesPacketParser(), status);
		    SendWorkMessage(parse_cmd_data_ready);
		    WaitMainMessage(cv);
		    assert(
			parse_cmd_data_consumed == cv ||
			parse_cmd_seq_end_received == cv);
		}
	    }
            break;

	case StreamState::pes_padding_stream:
	    {
		uint32_t len  = _fbitBuffer.GetBits(16);
		uint8_t* pbuf = new uint8_t[len];
		if (nullptr != pbuf) {
		    _fbitBuffer.GetBytes(pbuf, len);
		    delete [] pbuf;
		    pbuf = nullptr;
		}
	    }
	    break;

	case StreamState::program_end:
	    break;

        default:
	    cout << "Error: Invalid start code: 0x" << hex << static_cast<int>(cmd) << endl;
	    exit(-1);
        }
    } while (StreamState::program_end != _fbitBuffer.GetLastStartCode());

    return status;
}

uint32_t BaseParser::ParseMPEG2Stream(void)
{
    uint32_t status = 0;

    do {
	CHECK_PARSE(_seqHdrParser->ParseSequenceHdr(), status);
	if (StreamState::extension_start != _bbitBuffer.GetLastStartCode()) {
	    // MPEG1 stream
	    status = -1;
	    break;
	}

	CHECK_PARSE(_seqExtParser->ParseSequenceExt(), status);
	do {
	    CHECK_PARSE(ParseExtensionUserData(0), status);
	    do {
		if (StreamState::group_start == _bbitBuffer.GetLastStartCode()) {
		    CHECK_PARSE(_gopParser->ParseGopHdr(), status);
		    CHECK_PARSE(ParseExtensionUserData(1), status);
		}

		CHECK_PARSE(_picParser->ParsePictureHdr(), status);
		CHECK_PARSE(_picParser->ParsePictCodingExt(), status);
		CHECK_PARSE(ParseExtensionUserData(2), status);
		CHECK_PARSE(_picParser->ParsePictData(), status);
	    } while (StreamState::picture_start == _bbitBuffer.GetLastStartCode() ||
		     StreamState::group_start == _bbitBuffer.GetLastStartCode());

	    if (StreamState::sequence_end != _bbitBuffer.GetLastStartCode()) {
		CHECK_PARSE(_seqHdrParser->ParseSequenceHdr(), status);
		CHECK_PARSE(_seqExtParser->ParseSequenceExt(), status);
	    }
	} while (StreamState::sequence_end != _bbitBuffer.GetLastStartCode());
    } while (0);
    
    return status;
}

uint32_t BaseParser::ParseExtensionUserData(uint32_t flag)
{
    uint32_t status = 0;

    do {
	while ((_bbitBuffer.GetLastStartCode() == StreamState::extension_start) ||
	       (_bbitBuffer.GetLastStartCode() == StreamState::user_data_start)) {
	    if ((flag != 1) && (_bbitBuffer.GetLastStartCode() == StreamState::extension_start)) {
		CHECK_PARSE(_seqExtParser->ParseExtensionData(flag), status);
	    }
	    if (_bbitBuffer.GetLastStartCode() == StreamState::user_data_start) {
		CHECK_PARSE(_userDataParser->ParseUserData(), status);
	    }
	}
    } while(0);

    return status;
}

uint32_t BaseParser::SendMainMessage(uint32_t cmd)
{
    uint32_t status = 0;
    
    do {
	pthread_mutex_lock(&_mainDoorbellMutex);
	_parseCmd = cmd;
	pthread_cond_signal(&_mainDoorbellCond);
	pthread_mutex_unlock(&_mainDoorbellMutex);
    } while (0);

    return status;
}

uint32_t BaseParser::WaitMainMessage(uint32_t& cmd)
{
    uint32_t status = 0;
    
    do {
	// wait for the next command from the main thread
	cout << "Main waiting for next cmd..." << endl;
	pthread_cond_wait(&_mainDoorbellCond, &_mainDoorbellMutex);
	cmd = _parseCmd;
	DumpCommand(cmd);
    } while (0);
    
    return status;
}

uint32_t BaseParser::SendWorkMessage(uint32_t cmd)
{
    uint32_t status = 0;
    
    do {
	pthread_mutex_lock(&_workDoorbellMutex);
	_parseCmd = cmd;
	pthread_cond_signal(&_workDoorbellCond);
	pthread_mutex_unlock(&_workDoorbellMutex);
    } while (0);

    return status;
}

uint32_t BaseParser::WaitWorkMessage(uint32_t& cmd)
{
    uint32_t status = 0;
    
    do {
	// wait for the next command from the main thread
	cout << "Worker waiting for next cmd..." << endl;
	pthread_cond_wait(&_workDoorbellCond, &_workDoorbellMutex);
	cmd = _parseCmd;
	DumpCommand(cmd);
    } while (0);
    
    return status;
}

PackHdrParser* BaseParser::GetPackHdrParser(void)
{
    if (nullptr == _packHdrParser) {
        _packHdrParser = new PackHdrParser(_fbitBuffer, _streamState);
    }
    return _packHdrParser;
}

SystemHdrParser* BaseParser::GetSystemHdrParser(void)
{
    if (nullptr == _systemHdrParser) {
        _systemHdrParser = new SystemHdrParser(_fbitBuffer, _streamState);
    }
    return _systemHdrParser;
}

PesHeaderParser* BaseParser::GetPesHdrParser(void)
{
    if (nullptr == _pesHdrParser) {
        _pesHdrParser = new PesHeaderParser(_fbitBuffer, _streamState);
    }
    return _pesHdrParser;
}

PesPacketParser* BaseParser::GetPesPacketParser(void)
{
    if (nullptr == _pesPacketParser) {
        _pesPacketParser = new PesPacketParser(_bbitBuffer, _streamState);
    }
    return _pesPacketParser;
}

SeqHdrParser* BaseParser::GetSeqHdrParser(void)
{
    if (nullptr == _seqHdrParser) {
        _seqHdrParser = new SeqHdrParser(_bbitBuffer, _streamState);
    }
    return _seqHdrParser;
}

SeqExtParser* BaseParser::GetSeqExtParser(void)
{
    if (nullptr == _seqExtParser) {
        _seqExtParser = new SeqExtParser(_bbitBuffer, _streamState);
    }
    return _seqExtParser;
}

UserDataParser* BaseParser::GetUserDataParser(void)
{
    if (nullptr == _userDataParser) {
        _userDataParser = new UserDataParser(_bbitBuffer, _streamState);
    }
    return _userDataParser;
}

GopHdrParser* BaseParser::GetGopHdrParser(void)
{
    if (nullptr == _gopParser) {
        _gopParser = new GopHdrParser(_bbitBuffer, _streamState);
    }
    return _gopParser;
}

PictureParser* BaseParser::GetPictureParser(void)
{
    if (nullptr == _picParser) {
        _picParser = new PictureParser(_bbitBuffer, _streamState);
    }
    return _picParser;
}

SliceParser* BaseParser::GetSliceParser(void)
{
    if (nullptr == _sliceParser) {
        _sliceParser = new SliceParser(_bbitBuffer, _streamState);
    }
    return _sliceParser;
}
