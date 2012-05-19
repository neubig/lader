LANG=$1
NAME=$LANG-$2
DATA=$3

echo "~/work/lader/src/bin/lader -out_format parse -beam 1 -model stir/$LANG/$NAME.mod < data/$LANG-$DATA.dev > stir/$LANG/$NAME.devout"
time ~/work/lader/src/bin/lader -out_format parse -beam 1 -model stir/$LANG/$NAME.mod < data/$LANG-$DATA.dev > stir/$LANG/$NAME.devout
echo "~/usr/local/EVALB/evalb data/$LANG-unlabtree.dev stir/$LANG/$NAME.devout > stir/$LANG/$NAME.deveval"
~/usr/local/EVALB/evalb data/$LANG-unlabtree.dev stir/$LANG/$NAME.devout > stir/$LANG/$NAME.deveval
echo "~/work/lader/src/bin/lader -out_format parse -beam 1 -model stir/$LANG/$NAME.mod < data/$LANG-$DATA.test > stir/$LANG/$NAME.testout"
time ~/work/lader/src/bin/lader -out_format parse -beam 1 -model stir/$LANG/$NAME.mod < data/$LANG-$DATA.test > stir/$LANG/$NAME.testout
echo "~/usr/local/EVALB/evalb data/$LANG-unlabtree.test stir/$LANG/$NAME.testout > stir/$LANG/$NAME.testeval"
~/usr/local/EVALB/evalb data/$LANG-unlabtree.test stir/$LANG/$NAME.testout > stir/$LANG/$NAME.testeval

tail -n 1 stir/$LANG/$NAME.*eval
