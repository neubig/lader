COST=1e-3
LANG=ja-en
DATA=srccfg
LOSS=fuz
AMOUNT=dev
MOD=WOIQN-d$DATA-c$COST-l$LOSS-a$AMOUNT
NAME=$LANG-$MOD

# Set the attachment rule
if [[ $LANG == "en-ja" ]]; then
    ATTACH=right
else
    ATTACH=left
fi
# Set the loss
if [[ $LOSS == "fuz" ]]; then
    LOSS="fuzzy=1"
else 
    if [[ $LOSS == "tau" ]]; then
        LOSS="tau=0.1"
    else
        LOSS="fuzzy=0.5|tau=0.05"
    fi
fi

WD=`pwd`;
[[ -e test/$LANG ]] || mkdir test/$LANG

# With one sequence
# time src/bin/train-kyldr -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two sequences
# time src/bin/train-kyldr -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=straight-tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# With two seq+cfg
echo "time src/bin/train-kyldr -cost $COST -attach_null $ATTACH -loss_profile \"$LOSS\" -feature_profile \"seq=dict=straight-tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET\" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log"
time src/bin/train-kyldr -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=straight-tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With three sequences
# time src/bin/train-kyldr -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=straight-tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

script/test-kyldr.sh $LANG $MOD $DATA &> log/$NAME.log
