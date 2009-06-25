gdataatomincludedir = $(pkgincludedir)/gdata/atom
gdataatominclude_HEADERS = \
	gdata-author.h		\
	gdata-category.h	\
	gdata-generator.h	\
	gdata-link.h

noinst_LTLIBRARIES = libgdataatom.la

libgdataatom_la_SOURCES = \
	gdata-author.c		\
	gdata-category.c	\
	gdata-generator.c	\
	gdata-link.c

libgdataatom_la_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdata		\
	-I$(top_srcdir)/gdata/atom	\
	$(DISABLE_DEPRECATED)		\
	$(AM_CPPFLAGS)

libgdataatom_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdataatom_la_LIBADD = \
	$(GDATA_LIBS)

libgdataatom_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

-include $(top_srcdir)/git.mk
