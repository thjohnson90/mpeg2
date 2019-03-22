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

using namespace std;

struct test_case {
    uint64_t bits;
    uint32_t peek;
    uint32_t ecbp;
};

struct test_case cbp_test_cases[] = {
    {0x1C0, 9, 60},
    {0x1A0, 9,  4},
    {0x180, 9,  8},
    {0x160, 9, 16},
    {0x140, 9, 32},
    {0x130, 9, 12},
    {0x120, 9, 48},
    {0x110, 9, 20},
    {0x100, 9, 40},
    {0xF0,  9, 28},
    {0xE0,  9, 44},
    {0xD0,  9, 52},
    {0xC0,  9, 56},
    {0xB0,  9,  1},
    {0xA0,  9, 61},
    {0x90,  9,  2},
    {0x80,  9, 62},
    {0x78,  9, 24},
    {0x70,  9, 36},
    {0x68,  9,  3},
    {0x60,  9, 63},
    {0x5C,  9,  5},
    {0x58,  9,  9},
    {0x54,  9, 17},
    {0x50,  9, 33},
    {0x4C,  9,  6},
    {0x48,  9, 10},
    {0x44,  9, 18},
    {0x40,  9, 34},
    {0x3E,  9,  7},
    {0x3C,  9, 11},
    {0x3A,  9, 19},
    {0x38,  9, 35},
    {0x36,  9, 13},
    {0x34,  9, 49},
    {0x32,  9, 21},
    {0x30,  9, 41},
    {0x2E,  9, 14},
    {0x2C,  9, 50},
    {0x2A,  9, 22},
    {0x28,  9, 42},
    {0x26,  9, 15},
    {0x24,  9, 51},
    {0x22,  9, 23},
    {0x20,  9, 43},
    {0x1E,  9, 25},
    {0x1C,  9, 37},
    {0x1A,  9, 26},
    {0x18,  9, 38},
    {0x16,  9, 29},
    {0x14,  9, 45},
    {0x12,  9, 53},
    {0x10,  9, 57},
    {0xE,   9, 30},
    {0xC,   9, 46},
    {0xA,   9, 54},
    {0x8,   9, 58},
    {0x7,   9, 31},
    {0x6,   9, 47},
    {0x5,   9, 55},
    {0x4,   9, 59},
    {0x3,   9, 27},
    {0x2,   9, 39},
    {0x1,   9,  0}
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
    
    MacroblkParser mbp(bbf, ss);
    struct test_case* tc = nullptr;

    cout << "Coded Block Pattern Tests" << endl;
    for (i = 0; i < sizeof(cbp_test_cases) / sizeof(struct test_case); i++) {
	cout << "CBP Test Case " << dec << i+1 << ": ";
	tc = &cbp_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mbp.CodedBlkPattern(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->ecbp != pd.macroblkData.coded_block_pattern_420) {
	    cout
		<< "MB Type Error (got 0x"
		<< hex << pd.macroblkData.coded_block_pattern_420
		<< ", expected 0x"
		<< tc->ecbp << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    
    return 0;
}
