#include <iostream>
#include <stdint.h>
#include <assert.h>

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
    _inCallback(false)
{
}

BaseParser::~BaseParser()
{
    Destroy();
}

uint32_t BaseParser::Initialize(void)
{
    uint32_t status = 0;
    
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
	}
    }

    return status;
}

uint32_t BaseParser::Destroy(void)
{
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
}

static uint32_t calldepth = 0;
uint32_t BaseParser::ParseVideoSequence(void)
{
    uint32_t status   = 0;
    bool     exitLoop = false;

    cout << "Enter CD: " << ++calldepth << endl;

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
	    if (_inCallback) {
		exitLoop = false;
	    }
            break;

        case StreamState::system_header:
            CHECK_PARSE(_systemHdrParser->ParseSystemHdr(), status);
            break;

	case StreamState::pes_audio_stream_min:
	case StreamState::pes_video_stream_min:
        case StreamState::pes_padding_stream:
            CHECK_PARSE(_pesHdrParser->ParsePesHdr(), status);
	    if (StreamState::pes_video_stream_min == cmd) {
		cout << "PES Packet Size: 0x" << hex << _streamState.pesHdr.packet_len << endl;
		uint8_t* pbuf = _bbitBuffer.GetEmptyBuffer(_streamState.pesHdr.packet_len);
		if (nullptr != pbuf) {
		    _fbitBuffer.GetBytes(pbuf, _streamState.pesHdr.packet_len);
		    CHECK_PARSE(_pesPacketParser->ChoosePesPacketParser(), status);
		    _bbitBuffer.GetNextStartCode();
		    if (!_inCallback) {
			ParseMPEG2Stream();
		    } else {
			exitLoop = true;
		    }
		}
	    }
            break;

        default:
            if (cmd >= StreamState::slice_start_min && cmd <= StreamState::slice_start_max) {
                CHECK_PARSE(_sliceParser->ParseSliceData(), status);
                continue;
            } else {
		cout << "Error: Invalid start code: 0x" << hex << static_cast<int>(cmd) << endl;
		exit(-1);
	    }
            break;
        }

	if (-1 == status) {
	    break;
	}
    } while (StreamState::sequence_end != _fbitBuffer.GetLastStartCode() && !exitLoop);

    cout << "Exit CD: " << calldepth-- << endl;

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
	} while (0 /*StreamState::sequence_end != _bbitBuffer.GetLastStartCode()*/);
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
