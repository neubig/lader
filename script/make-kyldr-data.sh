#/bin/bash
set -e

SRC=tok-orig
DEST=tok-data

for f in a2000 a5000 a10000; do
    script/make-alignments.pl --max_len=60 $SRC/{japanese,english,align}-$f.txt $DEST/ja-en-{src,trg,align}.$f
    script/make-alignments.pl --reverse --max_len=60 $SRC/{english,japanese,align}-$f.txt $DEST/en-ja-{src,trg,align}.$f
    # Annotate classes
    script/add-classes.pl tables/ja.mc50 < $DEST/ja-en-src.$f > $DEST/ja-en-mc50.$f
    script/add-classes.pl tables/en.mc50 < $DEST/en-ja-src.$f > $DEST/en-ja-mc50.$f
    # Make japanese POSs
    cat $DEST/ja-en-src.$f | sed 's/\//／/g; s/&/＆/g' | kytea -nows -notag 2 | sed 's/\([^ ]*\)\/助詞/助詞-\1/g; s/[^ ]*\///g' > $DEST/ja-en-pos.$f
    # Combine
    script/join-features.pl $DEST/ja-en-src.$f $DEST/ja-en-pos.$f > $DEST/ja-en-srcpos.$f 
    script/join-features.pl $DEST/ja-en-src.$f $DEST/ja-en-mc50.$f > $DEST/ja-en-srcmc50.$f 
    script/join-features.pl $DEST/en-ja-src.$f $DEST/en-ja-mc50.$f > $DEST/en-ja-srcmc50.$f 
done
