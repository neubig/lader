COST=1e-4
LANG=en-ja
DATA=srcmc50
LOSS=fuz
AMOUNT=dev
MOD=WOIQCBANnor-d$DATA-c$COST-l$LOSS-a$AMOUNT
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
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two sequences + CL + S
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,CL%CL%ET,N%SN%ET,BIAS,S%SS5%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,S%SS8%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# With two sequences + CL + BA + Q
time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,Q#%SN%SQ#00,Q#%SN%SQ#00%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two sequences + CL + BA
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two sequences
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two sequences + Q#
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,Q#%SN%SQ#00,Q#%SN%SQ#00%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With one seq+cfg
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With two seq+cfg + all feats
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,Q#%SN%SQ#00,Q#%SN%SQ#00%ET,CL%CL%ET,B%SB%ET,A%SA%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET|cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

# # With three sequences
# time src/bin/train-kyldr -use_reverse false -cost $COST -attach_null $ATTACH -loss_profile "$LOSS" -feature_profile "seq=dict=$WD/tables/$LANG-phrase.txt,LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET,Q%SN%SQE0,Q%SN%SQE0%ET,N%SN%ET,BIAS|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET|seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,O%SL%SR%ET,I%LR%RL%ET" -iterations 500 -beam 50 -model_out test/$LANG/$NAME.mod -source_in data/$LANG-$DATA.$AMOUNT -align_in data/$LANG-align.$AMOUNT &> test/$LANG/$NAME.log

script/test-kyldr.sh $LANG $MOD $DATA &> log/$NAME.log
