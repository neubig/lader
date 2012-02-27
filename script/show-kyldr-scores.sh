#/bin/bash
echo "============ DEV ja-en =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/ja-en*.deveval | sed 's/(loss[^)]*)//g'
echo "============ DEV en-ja =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/en-ja*.deveval | sed 's/(loss[^)]*)//g'
echo "============ TEST ja-en =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/ja-en*.testeval stirlab/*/ja-en*.stirtesteval | sed 's/(loss[^)]*)//g'
echo "============ TEST en-ja =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/en-ja*.testeval  stirlab/*/en-ja*.stirtesteval | sed 's/(loss[^)]*)//g'
