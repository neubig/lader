KYLDRCPP = hyper-graph.cc combined-alignment.cc alignment.cc \
           feature-sequence.cc feature-set.cc feature-base.cc \
           feature-vector.cc reorderer-model.cc loss-fuzzy.cc \
           reorderer-trainer.cc loss-base.cc reorderer-runner.cc \
           ranks.cc reorderer-evaluator.cc loss-tau.cc dictionary.cc \
           feature-data-parse.cc target-span.cc feature-parse.cc \
           loss-bracket.cc


AM_CPPFLAGS = -I$(srcdir)/../include

lib_LTLIBRARIES = libkyldr.la

libkyldr_la_SOURCES = ${KYLDRCPP}
libkyldr_la_LDFLAGS = -version-info 0:0:0
