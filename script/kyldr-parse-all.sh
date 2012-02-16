#/bin/bash

SRC=en
TRG=ja
MOD=WOIQN-dsrcmc50-c1e-3-lfuz

[[ -e parsed/$SRC-$TRG-$MOD ]] || mkdir parsed/$SRC-$TRG-$MOD
for f in data/lowmc50/kyoto-{test,dev,tune}.$SRC data/lowmc50/kyoto-train.cln.[012]*.$SRC; do 
    f1=`basename $f`; 
    bsub "/home/neubig/usr/arcs/local/kyldr/src/bin/kyldr -out_format parse -beam 1 -model models/$SRC-$TRG-$MOD.mod < data/lowmc50/$f1 >  parsed/$SRC-$TRG-$MOD/$f1";
done
