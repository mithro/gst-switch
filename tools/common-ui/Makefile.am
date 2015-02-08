
gst_switch_ui_SOURCES = gstvideodisp.c \
  gstaudiovisual.c gstswitchclient.c
gst_switch_ui_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(GCOV_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) $(X_CFLAGS) $(GTK_CFLAGS) $(AM_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-ui\""
gst_switch_ui_LDFLAGS = $(GCOV_LFLAGS) $(GST_LIBS) $(GST_BASE_LIBS) \
  $(GST_PLUGINS_BASE_LIBS) $(GSTPB_BASE_LIBS) -lm
gst_switch_ui_LDADD = $(GST_LIBS) $(X_LIBS) $(LIBM) $(GTK_LIBS) $(GLIB_LIBS)

