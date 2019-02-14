#include <iostream>
#include <fstream>
#include <stdint.h>
#include <unistd.h>

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
#include "picdata.h"
#include "slice.h"
#include "base_parser.h"
#include "file_bitbuf.h"
#include "buf_bitbuf.h"

int main(int argc, char** argv)
{
    int status = 0;
    
    do {
        if (2 != argc) {
            cout << "usage: mpeg2 video_file_name" << endl;
	    status = -1;
            break;
        }

        cout << "Parsing file: " << argv[1] << endl;

        ifstream inFile(argv[1], ios::binary);
        if (!inFile.good()) {
            status = -1;
	    cout << "Cannot open file " << argv[1] << endl;
            break;
        }

	filebuf* pbuf = inFile.rdbuf();
	streamoff sz = pbuf->pubseekoff(0, inFile.end, inFile.in);
	pbuf->pubseekpos(0, inFile.in);

	cout << "File is " << sz << "bytes long." << endl;

 	StreamState streamState;
	FileBitBuffer fileBitBuf(inFile);
	BufBitBuffer  bufBitBuf;

	BaseParser parser(fileBitBuf, bufBitBuf, streamState);
	if (0 > parser.Initialize()) {
	    cout << "Parser initialization error!" << endl;
	    break;
	}
	bufBitBuf.SetBaseParser(&parser);
	status = parser.ParseVideoSequence();
	if (0 > status) {
	    cout << "Detected an MPEG1 Stream" << endl;
	    break;
	}
	PictureDataMgr::GetPictureDataMgr(streamState)->ReleasePictureDataMgr();
    } while (0);

    return status;
}
