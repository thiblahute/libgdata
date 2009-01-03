SUBDIRS = services . tests
DIST_SUBDIRS = services tests

lib_LTLIBRARIES = libgdata.la

gdataincludedir = $(pkgincludedir)/gdata
gdatainclude_HEADERS = \
	gdata.h		\
	gdata-entry.h	\
	gdata-feed.h	\
	gdata-service.h	\
	gdata-types.h

libgdata_la_SOURCES = \
	gdata-entry.c	\
	gdata-feed.c	\
	gdata-service.c	\
	gdata-types.c	\
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
	services/libgdataservices.la

libgdata_la_LDFLAGS = \
	-version-info $(GDATA_LT_VERSION)	\
	-export-symbols $(srcdir)/gdata.symbols \
	-no-undefined				\
	$(AM_LDFLAGS)

EXTRA_DIST = \
	gdata.symbols
