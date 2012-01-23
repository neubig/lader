KYLDRTH = test-kyldr.h test-base.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

bin_PROGRAMS = test-kyldr

test_kyldr_SOURCES = test-kyldr.cpp ${KYTH}
test_kyldr_LDADD = ../lib/libkyldr.la
