#include <iostream>
#include <pthread.h>

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
    uint32_t einc;
};

struct test_case B1_test_cases[] = {
    {0x400,  1},
    {0x300,  2},
    {0x200,  3},
    {0x180,  4},
    {0x100,  5},
    {0xC0,   6},
    {0x80,   7},
    {0x70,   8},
    {0x60,   9},
    {0x58,  10},
    {0x50,  11},
    {0x48,  12},
    {0x40,  13},
    {0x38,  14},
    {0x30,  15},
    {0x2E,  16},
    {0x2C,  17},
    {0x2A,  18},
    {0x28,  19},
    {0x26,  20},
    {0x24,  21},
    {0x23,  22},
    {0x22,  23},
    {0x21,  24},
    {0x20,  25},
    {0x1F,  26},
    {0x1E,  27},
    {0x1D,  28},
    {0x1C,  29},
    {0x1B,  30},
    {0x1A,  31},
    {0x19,  32},
    {0x18,  33}
};

int main(void)
{
    int32_t  status = 0;
    uint32_t i      = 0;
    uint32_t inc    = 0;

    BufBitBuffer bbf;
    StreamState  ss;
    
    MacroblkParser mp(bbf, ss);
    struct test_case* tc = nullptr;

    for (i = 0; i < sizeof(B1_test_cases) / sizeof(struct test_case); i++) {
	cout << "B1 Test Case " << dec << i+1 << ": ";
	tc = &B1_test_cases[i];
	
	bbf.PokeBits(tc->bits, 11);
	inc = mp.RunGetMacroblkAddrInc();
	if (inc != tc->einc) {
	    cout << "MB Addr Inc (got " << inc << ", exp " << tc->einc << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }
    
    return 0;
}
