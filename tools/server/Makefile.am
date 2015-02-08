
PROGRAM = gst-switch-srv

SOURCES = \
  gio/gsocketinputstream.c \
  gstcase.c \
  gstcomposite.c \
  gstrecorder.c \
  gstswitchcontroller.c \
  gstswitchopts.c \
  gstswitchserver.c


CFLAGS = \
  $(AM_CFLAGS) \
  $(GCOV_CFLAGS) \
  $(GST_BASE_CFLAGS) \
  $(GST_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-srv\""


LDFLAGS = \
  $(GCOV_LFLAGS) \
  $(GSTPB_BASE_LIBS) \
  $(GST_BASE_LIBS) \
  $(GST_LIBS) \
  $(GST_PLUGINS_BASE_LIBS)


LDADD = \
  $(GIO_LIBS) \
  $(LIBM)


