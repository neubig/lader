#/bin/bash

for f in tok/kyoto-{dev,test,tune}.en tok/kyoto-train.cln.[012]*.en tok/en-ja*; do
    f1=`basename $f`;
    echo $f1;
    bsub "~/usr/local/stanford-parser/lexparser-pcfg.sh tok/$f1 > stanford/$f1"
done
