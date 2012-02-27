#!/bin/bash
set -e

LANGPAIR=$1
NAME=$LANGPAIR-$2
DATA=$3
if [[ $LANGPAIR == "en-ja" ]]; then
    ATTACH=right
else
    ATTACH=left
fi
STIRDATA="${DATA/unlab}"
STIRNAME="${NAME/unlab}"
echo "STIRDATA: $STIRDATA STIRNAME: $STIRNAME"

# time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < data/$LANGPAIR-$DATA.dev > stirlab/$LANGPAIR/$NAME.devout
# ~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.dev stirlab/$LANGPAIR/$NAME.devout data/$LANGPAIR-src.dev data/$LANGPAIR-trg.dev > stirlab/$LANGPAIR/$NAME.deveval
# time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < data/$LANGPAIR-$DATA.test > stirlab/$LANGPAIR/$NAME.testout
# ~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.test stirlab/$LANGPAIR/$NAME.testout data/$LANGPAIR-src.test data/$LANGPAIR-trg.test > stirlab/$LANGPAIR/$NAME.testeval

echo "script/join-features.pl data/$LANGPAIR-$STIRDATA.dev stir/$LANGPAIR/$STIRNAME.devout > stirlab/$LANGPAIR/$NAME.stirdevin"
script/join-features.pl data/$LANGPAIR-$STIRDATA.dev stir/$LANGPAIR/$STIRNAME.devout > stirlab/$LANGPAIR/$NAME.stirdevin
echo "time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < stirlab/$LANGPAIR/$NAME.stirdevin > stirlab/$LANGPAIR/$NAME.stirdevout"
time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < stirlab/$LANGPAIR/$NAME.stirdevin > stirlab/$LANGPAIR/$NAME.stirdevout
echo "~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.dev stirlab/$LANGPAIR/$NAME.stirdevout data/$LANGPAIR-src.dev data/$LANGPAIR-trg.dev > stirlab/$LANGPAIR/$NAME.stirdeveval"
~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.dev stirlab/$LANGPAIR/$NAME.stirdevout data/$LANGPAIR-src.dev data/$LANGPAIR-trg.dev > stirlab/$LANGPAIR/$NAME.stirdeveval
echo "script/join-features.pl data/$LANGPAIR-$STIRDATA.test stir/$LANGPAIR/$STIRNAME.testout > stirlab/$LANGPAIR/$NAME.stirtestin"
script/join-features.pl data/$LANGPAIR-$STIRDATA.test stir/$LANGPAIR/$STIRNAME.testout > stirlab/$LANGPAIR/$NAME.stirtestin
echo "time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < stirlab/$LANGPAIR/$NAME.stirtestin > stirlab/$LANGPAIR/$NAME.stirtestout"
time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model stirlab/$LANGPAIR/$NAME.mod < stirlab/$LANGPAIR/$NAME.stirtestin > stirlab/$LANGPAIR/$NAME.stirtestout
echo "~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.test stirlab/$LANGPAIR/$NAME.stirtestout data/$LANGPAIR-src.test data/$LANGPAIR-trg.test > stirlab/$LANGPAIR/$NAME.stirtesteval"
~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANGPAIR-align.test stirlab/$LANGPAIR/$NAME.stirtestout data/$LANGPAIR-src.test data/$LANGPAIR-trg.test > stirlab/$LANGPAIR/$NAME.stirtesteval

tail -n 1 stirlab/$LANGPAIR/$NAME.*eval
