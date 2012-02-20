
AM_CPPFLAGS = -I$(srcdir)/../include

bin_PROGRAMS = train-kyldr kyldr evaluate-kyldr label-ranking

train_kyldr_SOURCES = train-kyldr.cc
train_kyldr_LDADD = ../lib/libkyldr.la -lboost_regex

kyldr_SOURCES = kyldr.cc
kyldr_LDADD = ../lib/libkyldr.la -lboost_regex

evaluate_kyldr_SOURCES = evaluate-kyldr.cc
evaluate_kyldr_LDADD = ../lib/libkyldr.la -lboost_regex

label_ranking_SOURCES = label-ranking.cc
label_ranking_LDADD = ../lib/libkyldr.la -lboost_regex
