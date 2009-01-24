gdataservicesincludedir = $(pkgincludedir)/gdata/services
gdataservicesinclude_HEADERS = \
	gdata-youtube-service.h		\
	gdata-youtube-video.h

noinst_LTLIBRARIES = libgdataservices.la

libgdataservices_headers =

libgdataservices_la_SOURCES = \
	gdata-youtube-service.c		\
	gdata-youtube-service.h		\
	gdata-youtube-video.c		\
	gdata-youtube-video.h

libgdataservices_la_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_builddir)/gdata			\
	-I$(top_builddir)/gdata/services	\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

libgdataservices_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdataservices_la_LIBADD = \
	$(GDATA_LIBS)

libgdataservices_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)
