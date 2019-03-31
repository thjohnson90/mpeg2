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
#include "picdata.h"
#include "motvecs.h"
#include "doorbell.h"
#include "thread.h"
#include "thrdcmds.h"
#include "videoproc.h"
#include "block.h"
#include "macroblk.h"
#include "slice.h"
#include "picture.h"
#include "doorbell.h"
#include "thread.h"
#include "base_parser.h"
#include "file_bitbuf.h"
#include "buf_bitbuf.h"
#include "thrdcmds.h"

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
    _cmd(common_cmd::null)
{
}

BaseParser::~BaseParser()
{
    Destroy();
}

void* BaseParser::ParserWorker(void* arg)
{
    BaseParser& parser = *static_cast<BaseParser*>(arg);
    
    parser._worker.Listen();
    assert(parse_cmd::data_ready == parser._worker.GetCmd());
    parser.ParseMPEG2Stream();

    parser.Ring(parse_cmd::seq_end_received);

    return arg;
}

int32_t BaseParser::Initialize(void)
{
    int32_t status = 0;
    
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
		status = _picParser->Initialize();
		if (-1 == status) {
		    Destroy();
		    break;
		}
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}

	if (-1 == status) {
	    break;
	}
	
	// initalize low-level parser thread
	status = _worker.Initialize(ParserWorker, this);
	if (0 != status) {
	    Destroy();
	    status = -1;
	    break;
	}
    } while (0);
    
    return status;
}

int32_t BaseParser::Destroy(void)
{
    int32_t status = 0;

    do {
	// destroy low-level parser thread
	_worker.Join(nullptr);
	
	// destroy parsers
	delete _packHdrParser;   _packHdrParser   = nullptr;
	delete _systemHdrParser; _systemHdrParser = nullptr;
	delete _pesHdrParser;    _pesHdrParser    = nullptr;
	delete _pesPacketParser; _pesPacketParser = nullptr;
	delete _seqHdrParser;    _seqHdrParser    = nullptr;
	delete _seqExtParser;    _seqExtParser    = nullptr;
	delete _userDataParser;  _userDataParser  = nullptr;
	delete _gopParser;       _gopParser       = nullptr;
	
	_picParser->Destroy();
	delete _picParser;       _picParser       = nullptr;
    } while (0);

    return status;
}

uint32_t BaseParser::ParseVideoSequence(void)
{
    int32_t status = 0;
    
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
		cout << "PES Packet Size: 0x" << hex << _streamState.pesHdr.packet_len << endl;
		CHECK_PARSE(_pesPacketParser->ChoosePesPacketParser(), status);
		uint8_t* pbuf = _bbitBuffer.GetEmptyBuffer(_streamState.pesHdr.packet_len);
		if (nullptr != pbuf) {
		    _fbitBuffer.GetBytes(pbuf, _streamState.pesHdr.packet_len);
		    _worker.Ring(parse_cmd::data_ready);
		    _bell.Listen();
		    assert(parse_cmd::data_consumed == _cmd ||
			   parse_cmd::seq_end_received == _cmd);
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

int32_t BaseParser::ParseMPEG2Stream(void)
{
    int32_t status = 0;

    do {
	_bbitBuffer.GetNextStartCode();
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

int32_t BaseParser::ParseExtensionUserData(uint32_t flag)
{
    int32_t status = 0;

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
        _pesPacketParser = new PesPacketParser(_fbitBuffer, _streamState);
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
