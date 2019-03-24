#! /bin/bash

rm -f cbp dct dctsz iscan mbinc mbtype motcod qscal

make -f cbp.mak
make -f dct.mak
make -f dctsz.mak
make -f iscan.mak
make -f mbinc.mak
make -f mbtype.mak
make -f motcod.mak
make -f qscal.mak

./cbp
./dct
./dctsz
./iscan
./mbinc
./mbtype
./motcod
./qscal
