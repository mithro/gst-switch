gst_switch_ptz_SOURCES = gstvideodisp.c gstswitchptz.c
gst_switch_ptz_CFLAGS = -g -ggdb $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(GCOV_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) $(X_CFLAGS) $(GTK_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-ptz\""
gst_switch_ptz_LDFLAGS = $(GCOV_LFLAGS) $(GST_LIBS) $(GST_BASE_LIBS) \
  $(GST_PLUGINS_BASE_LIBS) $(GST_PLUGINS_BAD_LIBS) $(GSTPB_BASE_LIBS)
gst_switch_ptz_LDADD = $(GST_LIBS) $(X_LIBS) $(LIBM) $(GTK_LIBS) \
  $(GLIB_LIBS) -lm \
  $(prefix)/lib/libgstcamcontrol-1.0.so
