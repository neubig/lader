
AM_CPPFLAGS = -I$(srcdir)/../include $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)


bin_PROGRAMS = train-lader lader evaluate-lader label-ranking

train_lader_SOURCES = train-lader.cc
train_lader_LDADD = ../lib/liblader.la

lader_SOURCES = lader.cc
lader_LDADD = ../lib/liblader.la

evaluate_lader_SOURCES = evaluate-lader.cc
evaluate_lader_LDADD = ../lib/liblader.la

label_ranking_SOURCES = label-ranking.cc
label_ranking_LDADD = ../lib/liblader.la
