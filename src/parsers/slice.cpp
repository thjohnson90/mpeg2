#include <iostream>
#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "motvecs.h"
#include "macroblk.h"
#include "slice.h"

SliceParser::SliceParser(BitBuffer& bb, StreamState& ss) :
    _macroblkParser(nullptr), _bitBuffer(bb), _streamState(ss)
{
}

SliceParser::~SliceParser()
{
    Destroy();
}

int32_t SliceParser::Initialize(void)
{
    int32_t status = 0;

    do {
	// create parsers
	if (nullptr == _macroblkParser) {
	    try {
		_macroblkParser = GetMacroblkParser();
		if (nullptr != _macroblkParser) {
		    _macroblkParser->Initialize();
		}
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}
    } while (0);

    return status;
}

int32_t SliceParser::Destroy(void)
{
    int32_t status = 0;

    do {
	if (nullptr != _macroblkParser) {
	    _macroblkParser->Destroy();
	}
	delete _macroblkParser;
	_macroblkParser = nullptr;
    } while (0);

    return status;
}

int32_t SliceParser::ParseSliceData(void)
{
    int32_t      status  = 0;
    uint32_t     marker  = 0;
    PictureData* picData = 0;
    
    do {
        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        
	// do we really need another buffer - already got one when parsing the pic header
        picData = picDataMgr->GetCurrentBuffer();
        if (0 == picData) {
            status = -1;
            break;
        }
        
        if (2800 < _streamState.seqHdr.vertical_sz) {
            picData->sliceData.slice_vertical_position_ext = _bitBuffer.GetBits(3);
        }

	if (true == _streamState.extData.seqScalExt.present) {
	    if (sequence_scalable_extension::data_partitioning ==
		_streamState.extData.seqScalExt.scalable_mode) {
		picData->sliceData.priority_breakpoint = _bitBuffer.GetBits(7);
	    }
	}

	picData->sliceData.quantizer_scale_code = _bitBuffer.GetBits(5);

	if (1 == _bitBuffer.PeekBits(1)) {
	    uint32_t reserved = 0;
	    
	    picData->sliceData.intra_slice_flag = _bitBuffer.GetBits(1);
	    picData->sliceData.intra_slice      = _bitBuffer.GetBits(1);
	    reserved                            = _bitBuffer.GetBits(7);
	    while (1 == _bitBuffer.PeekBits(1)) {
		picData->sliceData.extra_bit_slice         = _bitBuffer.GetBits(1);
		picData->sliceData.extra_information_slice = _bitBuffer.GetBits(8);
	    }
	}

	picData->sliceData.extra_bit_slice = _bitBuffer.GetBits(1);
	do {
	    _macroblkParser->ParseMacroblkData();
	} while (0 == _bitBuffer.PeekBits(23));
	
	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

MacroblkParser* SliceParser::GetMacroblkParser(void)
{
    if (nullptr == _macroblkParser) {
        _macroblkParser = new MacroblkParser(_bitBuffer, _streamState);
    }
    return _macroblkParser;
}
