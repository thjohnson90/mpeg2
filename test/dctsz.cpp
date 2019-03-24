#include <iostream>
#include <pthread.h>
#include <string.h>

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
#include "videoproc.h"
#include "block.h"
#include "macroblk.h"
#include "slice.h"
#include "picture.h"
#include "doorbell.h"
#include "thread.h"
#include "base_parser.h"
#include "buf_bitbuf.h"
#include "picdata.h"
#include "block.h"

using namespace std;

struct test_case {
    uint64_t bits;
    uint32_t peek;
    uint32_t dctsz;
};

struct test_case dctszlum_test_cases[] = {
    {0x100, 9,  0},
    {0x0,   9,  1},
    {0x80,  9,  2},
    {0x140, 9,  3},
    {0x180, 9,  4},
    {0x1C0, 9,  5},
    {0x1E0, 9,  6},
    {0x1F0, 9,  7},
    {0x1F8, 9,  8},
    {0x1FC, 9,  9},
    {0x1FE, 9, 10},
    {0x1FF, 9, 11}
};

struct test_case dctszchr_test_cases[] = {
    {0x0,   10,  0},
    {0x100, 10,  1},
    {0x200, 10,  2},
    {0x300, 10,  3},
    {0x380, 10,  4},
    {0x3C0, 10,  5},
    {0x3E0, 10,  6},
    {0x3F0, 10,  7},
    {0x3F8, 10,  8},
    {0x3FC, 10,  9},
    {0x3FE, 10, 10},
    {0x3FF, 10, 11}
};

int main(void)
{
    int32_t  status = 0;
    uint32_t i      = 0;
    uint32_t run    = 0;
    int32_t  sl     = 0;
    bool     eob    = false;
    bool     esc    = false;

    BufBitBuffer bbf;
    StreamState  ss;
    PictureData  pd;
    
    BlockParser bp(bbf, ss);
    struct test_case* tc = nullptr;

    memset(&ss, 0, sizeof(ss));
    memset(&pd, 0, sizeof(pd));

    for (i = 0; i < sizeof(dctszlum_test_cases) / sizeof(struct test_case); i++) {
	cout << "DCT Size Lum Test Case " << dec << i+1 << ": ";
	tc = &dctszlum_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = bp.RunGetDctSizeLuminance(&pd);
	if (pd.blkData.dct_dc_size_luminance != tc->dctsz) {
	    cout << "Failed (got 0x" << pd.blkData.dct_dc_size_luminance << ", expected 0x" << tc->dctsz << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    cout << endl;

    for (i = 0; i < sizeof(dctszchr_test_cases) / sizeof(struct test_case); i++) {
	cout << "DCT Size Chr Test Case " << dec << i+1 << ": ";
	tc = &dctszchr_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = bp.RunGetDctSizeChrominance(&pd);
	if (pd.blkData.dct_dc_size_chrominance != tc->dctsz) {
	    cout << "Failed (got 0x" << pd.blkData.dct_dc_size_chrominance << ", expected 0x" << tc->dctsz << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }
    
    return 0;
}
