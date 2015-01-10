plugin_LTLIBRARIES = libgstswitch.la libgstassess.la libgstinter.la

libgstswitch_la_SOURCES = gstswitchplugin.c \
  gsttcpmixsrc.c gstswitch.c gstconvbin.c
libgstswitch_la_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS) \
  -DLOG_PREFIX="\"./plugins\""
libgstswitch_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstswitch_la_LIBADD = $(GST_BASE_LIBS) $(GST_LIBS) $(GIO_LIBS)
libgstswitch_la_LIBTOOLFLAGS = --tag=disable-static


libgstassess_la_SOURCES = gstassessplugin.c gstassess.c
libgstassess_la_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS) \
  -DLOG_PREFIX="\"./plugins\""
libgstassess_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstassess_la_LIBADD = $(GST_BASE_LIBS) $(GST_LIBS) $(GIO_LIBS)
libgstassess_la_LIBTOOLFLAGS = --tag=disable-static

libgstinter_la_SOURCES = \
	gstinteraudiosink.c \
	gstinteraudiosrc.c \
	gstintersubsink.c \
	gstintersubsrc.c \
	gstintervideosink.c \
	gstintervideosrc.c \
	gstinter.c \
	gstintersurface.c

noinst_HEADERS = \
	gstinteraudiosink.h \
	gstinteraudiosrc.h \
	gstintersubsink.h \
	gstintersubsrc.h \
	gstintervideosink.h \
	gstintervideosrc.h \
	gstintersurface.h

libgstinter_la_CFLAGS = \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_BASE_CFLAGS) \
	$(GST_CFLAGS)

libgstinter_la_LIBADD = \
	$(GST_PLUGINS_BASE_LIBS) -lgstvideo-@GST_API_VERSION@ -lgstaudio-@GST_API_VERSION@ \
	$(GST_BASE_LIBS) \
	$(GST_LIBS) \
	$(LIBM)

