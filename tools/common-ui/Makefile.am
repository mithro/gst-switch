
PROGRAMS = gst-switch-ui
SOURCES = \
  gstvideodisp.c \
  gstaudiovisual.c \
  gstswitchclient.c

CFLAGS = \
  $(AM_CFLAGS) \
  $(GCOV_CFLAGS) \
  $(GST_BASE_CFLAGS) \
  $(GST_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) \
  $(GTK_CFLAGS) \
  $(X_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-ui\""

LDFLAGS = \
  $(GCOV_LFLAGS) \
  $(GSTPB_BASE_LIBS) \
  $(GST_BASE_LIBS) \
  $(GST_LIBS) \
  $(GST_PLUGINS_BASE_LIBS) \
  -lm

LDADD = \
  $(GLIB_LIBS)
  $(GST_LIBS) \
  $(GTK_LIBS) \
  $(LIBM) \
  $(X_LIBS)

