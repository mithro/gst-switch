
PROGRAM = gst-switch-ptz
SOURCES = \
  gstvideodisp.c \
  gstswitchptz.c

CFLAGS = \
  $(GCOV_CFLAGS) \
  $(GST_BASE_CFLAGS) \
  $(GST_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) \
  $(GTK_CFLAGS) \
  $(X_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-ptz\"" \
  -g \
  -ggdb


LDFLAGS = \
  $(GCOV_LFLAGS) \
  $(GSTPB_BASE_LIBS)
  $(GST_BASE_LIBS) \
  $(GST_LIBS) \
  $(GST_PLUGINS_BAD_LIBS) \
  $(GST_PLUGINS_BASE_LIBS)


LDADD = \
  $(GST_LIBS) \
  $(X_LIBS) \
  $(LIBM) \
  $(GTK_LIBS) \
  $(GLIB_LIBS) \
  -lm \
  $(prefix)/lib/libgstcamcontrol-1.0.so
