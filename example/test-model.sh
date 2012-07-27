#!/bin/bash
set -e

# This bash file provides an example of how to run lader and evaluate its
# accuracy. Before using this file, you must run train-model.sh to create
# the model to be used.

#############################################################################
# 1. Creating/combining annotations
#  Like train-model.sh, we need to create annotations for our input sentence.
#  This is the same as before, so read train-model.sh for more details.

echo "../script/add-classes.pl data/classes.en < data/test.en > output/test.en.class"
../script/add-classes.pl data/classes.en < data/test.en > output/test.en.class

echo "paste data/test.en output/test.en.class data/test.en.pos data/test.en.parse > output/test.en.annot"
paste data/test.en output/test.en.class data/test.en.pos data/test.en.parse > output/test.en.annot

#############################################################################
# 3. Running the reorderer
#
# Once we have our annotated input we can run the reorderer. To do so, we must
# simply specify the model file and pipe the file we want to reorder.
#
# The one thing we do need to decide is the output format. lader can output
# reordererd sentences ("string"), the latent parse tree ("parse") or the IDs
# of the reordered words in the original sentence. Let's output all of them
# for now.

echo "../src/bin/lader -model output/train.mod -out_format order,string,parse < output/test.en.annot > output/test.en.reordered"
../src/bin/lader -model output/train.mod -out_format order,string,parse < output/test.en.annot > output/test.en.reordered

#############################################################################
# 4. Evaluating the reordered output
#
# If we have a set of alignments for the test set, we can evaluate the accuracy
# of the reorderer. This is done with evaluate-lader, which takes as input
# the true alignment, the reorderer output, and optionally the source and target
# sentences. Note that for evaluation, we must choose out_format for lader where
# "order" comes before any others (as above)
# 
# Also note that we need to set -attach-null to the same value that we set
# during training. (In this case, we'll use the default, "right")


echo "../src/bin/evaluate-lader -attach_null right data/test.en-ja.align output/test.en.reordered data/test.en > output/test.en.grade"
../src/bin/evaluate-lader -attach_null right data/test.en-ja.align output/test.en.reordered data/test.en > output/test.en.grade

tail -n 3 output/test.en.grade
