#! /bin/bash

rm -f cbp dct dctsz mbinc mbinc mbtype motcod

make -f cbp.mak
make -f dct.mak
make -f dctsz.mak
make -f mbinc.mak
make -f mbtype.mak
make -f motcod.mak

./cbp
./dct
./dctsz
./mbinc
./mbtype
./motcod
