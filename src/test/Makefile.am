LADERTH = test-alignments-and-ranks.h test-feature-vector.h test-base.h test-hyper-graph.h test-dictionary.h test-loss-bracket.h test-feature-align.h test-loss-chunk.h test-feature-parse.h test-loss-tau.h test-feature-sequence.h test-reorderer-model.h test-feature-set.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"' $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)

bin_PROGRAMS = test-lader

test_lader_SOURCES = test-lader.cc ${LADERTH}
test_lader_LDADD = ../lib/liblader.la -lboost_regex-mt -lboost_thread-mt -lboost_filesystem -lboost_system -lrt