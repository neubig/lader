LANG=$1
NAME=$LANG-$2
DATA=$3
if [[ $LANG == "en-ja" ]]; then
    ATTACH=right
else
    ATTACH=left
fi

echo "~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model test/$LANG/$NAME.mod < data/$LANG-$DATA.dev > test/$LANG/$NAME.devout"
time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model test/$LANG/$NAME.mod < data/$LANG-$DATA.dev > test/$LANG/$NAME.devout
echo "~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANG-align.dev test/$LANG/$NAME.devout data/$LANG-src.dev data/$LANG-trg.dev > test/$LANG/$NAME.deveval"
~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANG-align.dev test/$LANG/$NAME.devout data/$LANG-src.dev data/$LANG-trg.dev > test/$LANG/$NAME.deveval
echo "~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model test/$LANG/$NAME.mod < data/$LANG-$DATA.test > test/$LANG/$NAME.testout"
time ~/work/kyldr/src/bin/kyldr -out_format order,string,parse -beam 1 -model test/$LANG/$NAME.mod < data/$LANG-$DATA.test > test/$LANG/$NAME.testout
echo "~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANG-align.test test/$LANG/$NAME.testout data/$LANG-src.test data/$LANG-trg.test > test/$LANG/$NAME.testeval"
~/work/kyldr/src/bin/evaluate-kyldr -attach_null $ATTACH data/$LANG-align.test test/$LANG/$NAME.testout data/$LANG-src.test data/$LANG-trg.test > test/$LANG/$NAME.testeval

tail -n 1 test/$LANG/$NAME.*eval
