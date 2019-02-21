#include <iostream>
#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "motvecs.h"
#include "macroblk.h"

MotionVecsParser::MotionVecsParser(BitBuffer& bb, StreamState& ss) :
    _motionvecsParser(nullptr), _bitBuffer(bb), _streamState(ss)
{
}

MotionVecsParser::~MotionVecsParser()
{
    Destroy();
}

int32_t MotionVecsParser::Initialize(void)
{
    int32_t status = 0;

#if 0
    do {
      // create parsers
	if (nullptr == _motionvecsParser) {
	    try {
		_motionvecsParser = GetMotionVectorsParser();
		if (nullptr != _motionvecsParser) {
		    _motionvecsParser->Initalize();
		}
	    } catch (std::bad_alloc) {
		Destroy();
		status = -1;
		break;
	    }
	}
    } while (0);
#endif
    
    return status;
}

int32_t MotionVecsParser::Destroy(void)
{
    int32_t status = 0;

#if 0
    do {
      if (nullptr != _motionvecsParser) {
	    _motionvecsParser->Destroy();
	}
	delete _motionvecsParser;
	_motionvecsParser = nullptr;
    } while (0);
#endif
    
    return status;
}

int32_t MotionVecsParser::ParseMotionVecs(uint32_t s)
{
    int32_t      status  = 0;
    uint32_t     marker  = 0;
    PictureData* picData = 0;

    do {
	uint32_t bitCnt = 0;
	uint32_t mvcnt  = 0;

        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        
        picData = picDataMgr->GetCurrentBuffer();
        if (0 == picData) {
            status = -1;
            break;
        }

	if (0 != picData->macroblkData.frame_motion_type) {
	    switch(picData->macroblkData.frame_motion_type) {
	    }
	}
	
	if (0 != picData->macroblkData.field_motion_type) {
	    switch(picData->macroblkData.field_motion_type) {
	    }
	}

//	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

MotionVecsParser* MotionVecsParser::GetMotionVecsParser(void)
{
    if (nullptr == _motionvecsParser) {
        _motionvecsParser = new MotionVecsParser(_bitBuffer, _streamState);
    }
    return _motionvecsParser;
}
