
gst_switch_cap_SOURCES = gstswitchcapture.c gstswitchclient.c
gst_switch_cap_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(GCOV_CFLAGS) \
  $(GST_PLUGINS_BASE_CFLAGS) $(X_CFLAGS) $(GTK_CFLAGS) \
  -DLOG_PREFIX="\"gst-switch-cap\""
gst_switch_cap_LDFLAGS = $(GCOV_LFLAGS) $(GST_LIBS) $(GST_BASE_LIBS) \
  $(GST_PLUGINS_BASE_LIBS) $(GSTPB_BASE_LIBS) -lm
gst_switch_cap_LDADD = $(GST_LIBS) $(X_LIBS) $(LIBM) $(GTK_LIBS) $(GLIB_LIBS)
