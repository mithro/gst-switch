SUBDIRS = \
  capture \
  common-ui \
  server \
  support \
  ui

if SPEAKERTRACK_ENABLED
SUBDIRS += \
  ptz
endif

if GCOV_ENABLED
# --coverage
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
GCOV_LFLAGS = -fprofile-arcs -ftest-coverage
#GCOV_CFLAGS = --coverage
#GCOV_LFLAGS = --coverage
else
GCOV_CFLAGS =
GCOV_LFLAGS =
endif

if DEBUG
AM_CFLAGS = -g3 -O0 -gdwarf-4 -fvar-tracking-assignments -fvar-tracking
else
AM_CFLAGS = -O2
endif
