gdatayoutubeincludedir = $(pkgincludedir)/gdata/services/youtube
gdatayoutubeinclude_HEADERS = \
	gdata-youtube-service.h		\
	gdata-youtube-video.h		\
	gdata-youtube.h

noinst_LTLIBRARIES = libgdatayoutube.la

libgdatayoutube_headers =

libgdatayoutube_la_SOURCES = \
	gdata-youtube-service.c		\
	gdata-youtube-service.h		\
	gdata-youtube-video.c		\
	gdata-youtube-video.h		\
	gdata-youtube.c			\
	gdata-youtube.h

libgdatayoutube_la_CPPFLAGS = \
	-I$(top_srcdir)					\
	-I$(top_builddir)/gdata				\
	-I$(top_builddir)/gdata/services/youtube	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)

libgdatayoutube_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatayoutube_la_LIBADD = \
	$(GDATA_LIBS)

libgdatayoutube_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)
