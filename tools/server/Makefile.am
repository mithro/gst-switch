gst_switch_srv_SOURCES = gstswitchserver.c gstcase.c \
  gstcomposite.c gstswitchcontroller.c gstrecorder.c \
  gio/gsocketinputstream.c gstswitchopts.c
gst_switch_srv_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(GCOV_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) $(AM_CFLAGS) -DLOG_PREFIX="\"gst-switch-srv\""
gst_switch_srv_LDFLAGS = $(GCOV_LFLAGS) $(GST_LIBS) $(GST_BASE_LIBS) \
  $(GST_PLUGINS_BASE_LIBS) $(GSTPB_BASE_LIBS)
gst_switch_srv_LDADD = $(GIO_LIBS) $(LIBM)
