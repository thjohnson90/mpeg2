#include <stdint.h>
#include <pthread.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "doorbell.h"
#include "thread.h"
#include "thrdcmds.h"
#include "videoproc.h"
#include "sequence.h"

extern const int32_t dflt_intra_quant_matrix[8][8];
extern const int32_t dflt_non_intra_quant_matrix[8][8];

SeqHdrParser::SeqHdrParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t SeqHdrParser::ParseSequenceHdr(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    int32_t  v      = 0;
    int32_t  u      = 0;
    
    do {
	VideoProcessor* vidProc = VideoProcessor::GetInstance();
	
	// reset default quantization matrix pointers
	for (v = 0; v < 8; v++) {
	    for (u = 0; u < 8; u++) {
		_streamState.seqHdr.W[0][v][u] = dflt_intra_quant_matrix[v][u];
		_streamState.seqHdr.W[1][v][u] = dflt_non_intra_quant_matrix[v][u];
	    }
	}

        _streamState.seqHdr.horizontal_sz           = _bitBuffer.GetBits(12);
        _streamState.seqHdr.vertical_sz             = _bitBuffer.GetBits(12);
        _streamState.seqHdr.aspect_ratio            = _bitBuffer.GetBits(4);
        _streamState.seqHdr.frame_rate              = _bitBuffer.GetBits(4);
        _streamState.seqHdr.bit_rate                = _bitBuffer.GetBits(18);
        marker                                      = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.seqHdr.VBV_buffer_sz           = _bitBuffer.GetBits(10);
        _streamState.seqHdr.constrained_params_flag = _bitBuffer.GetBits(1);
        _streamState.seqHdr.load_intra_quant_matrix = _bitBuffer.GetBits(1);
        if (1 == _streamState.seqHdr.load_intra_quant_matrix) {
            // load the intra_quantization_matrix
            status = LoadQuantMatrix(_streamState.seqHdr.intra_quant_matrix);
	    if (0 > status) {
		break;
	    }
	    status = vidProc->GetAlternateScan(_streamState.seqHdr.intra_quant_matrix,
					       _streamState.seqHdr.W[0],
					       0);
	    if (0 > status) {
		break;
	    }
        }
        _streamState.seqHdr.load_non_intra_quant_matrix = _bitBuffer.GetBits(1);
        if (1 == _streamState.seqHdr.load_non_intra_quant_matrix) {
            // load the non_intra_quantization_matrix
            status = LoadQuantMatrix(_streamState.seqHdr.non_intra_quant_matrix);
	    if (0 > status) {
		break;
	    }
	    status = vidProc->GetAlternateScan(_streamState.seqHdr.non_intra_quant_matrix,
					       _streamState.seqHdr.W[1],
					       0);
	    if (0 > status) {
		break;
	    }
        }

	_bitBuffer.GetNextStartCode();
    } while (0);
    
    return status;
}

int32_t SeqHdrParser::LoadQuantMatrix(int32_t (&q)[64])
{
    int32_t status = 0;
    int     i      = 0;

    do {
	for (i = 0; i < 64; i++) {
	    q[i] = static_cast<uint8_t>(_bitBuffer.GetBits(8));
	}
    } while (0);

    return status;
}
