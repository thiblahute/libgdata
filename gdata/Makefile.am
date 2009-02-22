SUBDIRS = services . tests
DIST_SUBDIRS = services tests

# Marshalling
GDATA_MARSHAL_FILES = \
	gdata-marshal.c	\
	gdata-marshal.h

GLIB_GENMARSHAL = `pkg-config --variable=glib_genmarshal glib-2.0`

gdata-marshal.h: gdata-marshal.list Makefile
	( $(GLIB_GENMARSHAL) --prefix=gdata_marshal $(srcdir)/gdata-marshal.list --header > gdata-marshal.h )
gdata-marshal.c: gdata-marshal.h Makefile
	( $(GLIB_GENMARSHAL) --prefix=gdata_marshal $(srcdir)/gdata-marshal.list --header --body > gdata-marshal.c )

# libgdata library
lib_LTLIBRARIES = libgdata.la

gdataincludedir = $(pkgincludedir)/gdata
gdatainclude_HEADERS = \
	gdata.h			\
	gdata-entry.h		\
	gdata-feed.h		\
	gdata-service.h		\
	gdata-types.h		\
	gdata-query.h		\
	gdata-marshal.h		\
	gdata-atom.h		\
	gdata-media-rss.h	\
	gdata-gdata.h		\
	gdata-parser.h

libgdata_la_SOURCES = \
	$(GDATA_MARSHAL_FILES)	\
	gdata-entry.c		\
	gdata-feed.c		\
	gdata-service.c		\
	gdata-types.c		\
	gdata-query.c		\
	gdata-atom.c		\
	gdata-media-rss.c	\
	gdata-gdata.c		\
	gdata-parser.c		\
	gdata-private.h

libgdata_la_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_builddir)/gdata		\
	$(DISABLE_DEPRECATED)		\
	$(AM_CPPFLAGS)

libgdata_la_CFLAGS = \
	$(GDATA_CFLAGS)			\
	$(WARN_CFLAGS)			\
	$(AM_CFLAGS)			\
	-D_GNU_SOURCE

libgdata_la_LIBADD = \
	$(GDATA_LIBS)	\
	services/youtube/libgdatayoutube.la	\
	services/calendar/libgdatacalendar.la

libgdata_la_LDFLAGS = \
	-version-info $(GDATA_LT_VERSION)	\
	-export-symbols $(srcdir)/gdata.symbols \
	-no-undefined				\
	$(AM_LDFLAGS)

# General cleanup
CLEANFILES = $(GDATA_MARSHAL_FILES)
EXTRA_DIST = \
	gdata.symbols	\
	gdata-marshal.list
