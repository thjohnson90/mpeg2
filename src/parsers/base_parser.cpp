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

BaseParser::BaseParser(BitBuffer& bb, StreamState& ss) :
    _bitBuffer(bb), _streamState(ss), _packHdrParser(nullptr),
    _systemHdrParser(nullptr), _pesHdrParser(nullptr),
    _seqHdrParser(nullptr), _seqExtParser(nullptr),
    _userDataParser(nullptr), _gopParser(nullptr),
    _picParser(nullptr), _sliceParser(nullptr)
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
    delete _seqHdrParser;    _seqHdrParser    = nullptr;
    delete _seqExtParser;    _seqExtParser    = nullptr;
    delete _userDataParser;  _userDataParser  = nullptr;
    delete _gopParser;       _gopParser       = nullptr;
    delete _picParser;       _picParser       = nullptr;
    delete _sliceParser;     _sliceParser     = nullptr;
}

uint32_t BaseParser::ParseVideoSequence(void)
{
    uint32_t status = 0;
    
    do
    {
        unsigned char cmd = _bitBuffer.GetNextStartCode();
        
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
        case StreamState::pes_padding_stream:
            CHECK_PARSE(_pesHdrParser->ParsePesHdr(), status);
            break;

        case StreamState::sequence_header:
	    //
	    // _streamState.pesHdr.packet_len contains the PES packet length
	    //
            CHECK_PARSE(_seqHdrParser->ParseSequenceHdr(), status);
	    if (StreamState::extension_start == _bitBuffer.GetLastStartCode())
	    {
		// This is an MPEG2 stream
		CHECK_PARSE(_seqExtParser->ParseSequenceExt(), status);
		CHECK_PARSE(ParseMPEG2Stream(), status);
	    } else {
		// This is an MPEG1 stream
		status = -1;
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
    } while (0 <= status && StreamState::sequence_end != _bitBuffer.GetLastStartCode());
    
    return status;
}

uint32_t BaseParser::ParseMPEG2Stream(void)
{
    uint32_t status = 0;
    
    do {
	do {
	    CHECK_PARSE(ParseExtensionUserData(0), status);
	    
	    do {
		if (_bitBuffer.GetLastStartCode() == StreamState::group_start) {
		    CHECK_PARSE(_gopParser->ParseGopHdr(), status);
		    CHECK_PARSE(ParseExtensionUserData(1), status);
		}

		if (_bitBuffer.GetLastStartCode() == StreamState::picture_start) {
		    CHECK_PARSE(_picParser->ParsePictureHdr(), status);
		}

		if (_bitBuffer.GetLastStartCode() == StreamState::extension_start) {
		    CHECK_PARSE(_picParser->ParsePictCodingExt(), status);
		}
		CHECK_PARSE(ParseExtensionUserData(2), status);
		CHECK_PARSE(_picParser->ParsePictData(), status);
	    } while ((_bitBuffer.GetLastStartCode() == StreamState::picture_start) ||
		     (_bitBuffer.GetLastStartCode() == StreamState::group_start));

	    if (0 > status) {
	      break;
	    }

	    if (_bitBuffer.GetLastStartCode() == StreamState::sequence_header) {
		CHECK_PARSE(_seqHdrParser->ParseSequenceHdr(), status);
		CHECK_PARSE(_seqExtParser->ParseSequenceExt(), status);
	    }

	    if (_bitBuffer.GetLastStartCode() == StreamState::pack_header) {
		CHECK_PARSE(_packHdrParser->ParsePackHdr(), status);
		assert(StreamState::pes_video_stream_min == _bitBuffer.GetNextStartCode());
		CHECK_PARSE(_pesHdrParser->ParsePesHdr(), status);
		_bitBuffer.GetNextStartCode();
	    }

	    if (_bitBuffer.GetLastStartCode() == StreamState::extension_start) {
		CHECK_PARSE(_picParser->ParsePictCodingExt(), status);
	    }
	} while (_bitBuffer.GetLastStartCode() != StreamState::sequence_end);
	
    } while (0);
    
    return status;
}

uint32_t BaseParser::ParseExtensionUserData(uint32_t flag)
{
    uint32_t status = 0;

    do {
	while ((_bitBuffer.GetLastStartCode() == StreamState::extension_start) ||
	       (_bitBuffer.GetLastStartCode() == StreamState::user_data_start)) {
	    if ((flag != 1) && (_bitBuffer.GetLastStartCode() == StreamState::extension_start)) {
		CHECK_PARSE(_seqExtParser->ParseExtensionData(flag), status);
	    }
	    if (_bitBuffer.GetLastStartCode() == StreamState::user_data_start) {
		CHECK_PARSE(_userDataParser->ParseUserData(), status);
	    }
	}
    } while(0);

    return status;
}

PackHdrParser* BaseParser::GetPackHdrParser(void)
{
    if (nullptr == _packHdrParser) {
        _packHdrParser = new PackHdrParser(_bitBuffer, _streamState);
    }
    return _packHdrParser;
}

SystemHdrParser* BaseParser::GetSystemHdrParser(void)
{
    if (nullptr == _systemHdrParser) {
        _systemHdrParser = new SystemHdrParser(_bitBuffer, _streamState);
    }
    return _systemHdrParser;
}

PesHeaderParser* BaseParser::GetPesHdrParser(void)
{
    if (nullptr == _pesHdrParser) {
        _pesHdrParser = new PesHeaderParser(_bitBuffer, _streamState);
    }
    return _pesHdrParser;
}

SeqHdrParser* BaseParser::GetSeqHdrParser(void)
{
    if (nullptr == _seqHdrParser) {
        _seqHdrParser = new SeqHdrParser(_bitBuffer, _streamState);
    }
    return _seqHdrParser;
}

SeqExtParser* BaseParser::GetSeqExtParser(void)
{
    if (nullptr == _seqExtParser) {
        _seqExtParser = new SeqExtParser(_bitBuffer, _streamState);
    }
    return _seqExtParser;
}

UserDataParser* BaseParser::GetUserDataParser(void)
{
    if (nullptr == _userDataParser) {
        _userDataParser = new UserDataParser(_bitBuffer, _streamState);
    }
    return _userDataParser;
}

GopHdrParser* BaseParser::GetGopHdrParser(void)
{
    if (nullptr == _gopParser) {
        _gopParser = new GopHdrParser(_bitBuffer, _streamState);
    }
    return _gopParser;
}

PictureParser* BaseParser::GetPictureParser(void)
{
    if (nullptr == _picParser) {
        _picParser = new PictureParser(_bitBuffer, _streamState);
    }
    return _picParser;
}

SliceParser* BaseParser::GetSliceParser(void)
{
    if (nullptr == _sliceParser) {
        _sliceParser = new SliceParser(_bitBuffer, _streamState);
    }
    return _sliceParser;
}
