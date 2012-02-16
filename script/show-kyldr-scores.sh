#/bin/bash
echo "============ DEV =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/*.deveval
echo "============ TEST =============="
grep "loss [0-9]*\/[0-9][0-9][0-9][0-9][0-9]" test/*/*.testeval
