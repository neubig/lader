LADERTH = test-lader.h test-base.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

bin_PROGRAMS = test-lader

test_lader_SOURCES = test-lader.cc ${KYTH}
test_lader_LDADD = ../lib/liblader.la -lboost_regex
