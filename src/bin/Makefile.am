
AM_CPPFLAGS = -I$(srcdir)/../include

bin_PROGRAMS = train-lader lader evaluate-lader label-ranking

train_lader_SOURCES = train-lader.cc
train_lader_LDADD = ../lib/liblader.la -lboost_regex-mt -lboost_thread-mt -lrt

lader_SOURCES = lader.cc
lader_LDADD = ../lib/liblader.la -lboost_regex-mt -lboost_thread-mt -lrt

evaluate_lader_SOURCES = evaluate-lader.cc
evaluate_lader_LDADD = ../lib/liblader.la -lboost_regex-mt -lboost_thread-mt -lrt

label_ranking_SOURCES = label-ranking.cc
label_ranking_LDADD = ../lib/liblader.la -lboost_regex-mt -lboost_thread-mt -lrt
