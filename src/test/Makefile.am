KYLDRTH = test-kyldr.h test-base.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

bin_PROGRAMS = test-kyldr

test_kyldr_SOURCES = test-kyldr.cc ${KYTH}
test_kyldr_LDADD = ../lib/libkyldr.la
