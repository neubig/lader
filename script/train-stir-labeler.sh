COST=1e-3
LANG=en-ja
DATA=srcmc50cfgunlab
LOSS=fuz
AMOUNT=dev
MOD=WOIGCBANnor-d$DATA-c$COST-l$LOSS-a$AMOUNT
NAME=$LANG-$MOD

# Set the attachment rule
if [[ $LANG == "en-ja" ]]; then
    ATTACH=right
else
    ATTACH=left
fi
# Set the loss
LOSS="bracket=1"

WD=`pwd`;
[[ -e test/$LANG ]] || mkdir test/$LANG

# # With two sequences + CL + BA + G / CFG
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-contiguous.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SQE0%ET,Q0%SQ#00%ET,Q1%SQ#01%ET,Q2%SQ#02%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET|cfg=SP%SP%ET" -iterations 500 -beam 50 -model_out stirlab/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -parse_in data/$LANG-stirtree.$AMOUNT &> stirlab/$LANG/$NAME.log

# # With three sequences + CL + BA + G / CFG
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-contiguous.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SQE0%ET,Q0%SQ#00%ET,Q1%SQ#01%ET,Q2%SQ#02%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET|cfg=SP%SP%ET" -iterations 500 -beam 50 -model_out stirlab/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -parse_in data/$LANG-stirtree.$AMOUNT &> stirlab/$LANG/$NAME.log

# With two sequences + CL + BA + G / two CFG
time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-contiguous.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SQE0%ET,Q0%SQ#00%ET,Q1%SQ#01%ET,Q2%SQ#02%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET|cfg=SP%SP%ET" -iterations 500 -beam 50 -model_out stirlab/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -parse_in data/$LANG-stirtree.$AMOUNT &> stirlab/$LANG/$NAME.log

# # With one seq+cfg
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log


script/test-stir-labeler.sh $LANG $MOD $DATA &> log/$NAME.log
