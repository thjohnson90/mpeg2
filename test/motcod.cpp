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
#include "motvecs.h"

using namespace std;

struct test_case {
    uint64_t bits;
    uint32_t peek;
    int32_t  emc;
};

struct test_case motcod_test_cases[] = {
    {0x18, 11,  16},
    {0x19, 11, -16},
    {0x1A, 11,  15},
    {0x1B, 11, -15},
    {0x1C, 11,  14},
    {0x1D, 11, -14},
    {0x1E, 11,  13},
    {0x1F, 11, -13},
    {0x20, 11,  12},
    {0x21, 11, -12},
    {0x22, 11,  11},
    {0x23, 11, -11},
    {0x24, 11,  10},
    {0x26, 11, -10},
    {0x28, 11,   9},
    {0x2A, 11,  -9},
    {0x2C, 11,   8},
    {0x2E, 11,  -8},
    {0x30, 11,   7},
    {0x38, 11,  -7},
    {0x40, 11,   6},
    {0x48, 11,  -6},
    {0x50, 11,   5},
    {0x58, 11,  -5},
    {0x60, 11,   4},
    {0x70, 11,  -4},
    {0x80, 11,   3},
    {0xC0, 11,  -3},
    {0x100, 11,  2},
    {0x180, 11, -2},
    {0x200, 11,  1},
    {0x300, 11, -1},
    {0x400, 11,  0}
};

int main(void)
{
    int32_t  status = 0;
    uint32_t i      = 0;
    uint32_t inc    = 0;

    BufBitBuffer bbf;
    StreamState  ss;
    PictureData  pd;

    memset(&ss, 0, sizeof(ss));
    memset(&pd, 0, sizeof(pd));
    
    MotionVecsParser mv(bbf, ss);
    struct test_case* tc = nullptr;

    cout << "Motion Code Cases" << endl;
    for (i = 0; i < sizeof(motcod_test_cases) / sizeof(struct test_case); i++) {
	cout << "Motion Code Test Case " << dec << i+1 << ": ";
	tc = &motcod_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mv.RunGetMotionCode(&pd, 0, 0, 0);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->emc != pd.mvData.motion_code[0][0][0]) {
	    cout << "Motion Code Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }
    
    return 0;
}
