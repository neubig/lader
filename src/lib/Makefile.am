KYLDRCPP = combined-alignment.cc hyper-graph.cc hyper-node.cc \
           feature-sequence.cc feature-vector.cc alignment.cc \
           reorderer-trainer.cc feature-set.cc feature-base.cc \
           loss-fuzzy.cc


AM_CPPFLAGS = -I$(srcdir)/../include

lib_LTLIBRARIES = libkyldr.la

libkyldr_la_SOURCES = ${KYLDRCPP}
libkyldr_la_LDFLAGS = -version-info 0:0:0
