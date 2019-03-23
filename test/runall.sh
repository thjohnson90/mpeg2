#! /bin/bash

rm -f cbp dct dctsz mbinc mbinc mbtype motcod iscan

make -f cbp.mak
make -f dct.mak
make -f dctsz.mak
make -f mbinc.mak
make -f mbtype.mak
make -f motcod.mak
make -f iscan.mak

./cbp
./dct
./dctsz
./mbinc
./mbtype
./motcod
./iscan
