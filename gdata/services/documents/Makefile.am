gdatadocumentsincludedir = $(pkgincludedir)/gdata/services/documents
gdatadocumentsinclude_HEADERS = \
	gdata-documents-service.h	\
	gdata-documents-feed.h	\
	gdata-documents-entry.h	\
	gdata-documents-query.h
	gdata-documents-text.h	\
	gdata-documents-presentation.h	\
	gdata-documents-folder.h	\
	gdata-documents-spreadsheet.h

noinst_LTLIBRARIES = libgdatadocuments.la

libgdatadocuments_la_SOURCES = \
	gdata-documents-service.c	\
	gdata-documents-service.h	\
	gdata-documents-feed.c	\
	gdata-documents-feed.h	\
	gdata-documents-entry.c	\
	gdata-documents-entry.h	\
	gdata-documents-text.c	\
	gdata-documents-text.h	\
	gdata-documents-presentation.c	\
	gdata-documents-presentation.h	\
	gdata-documents-spreadsheet.c	\
	gdata-documents-spreadsheet.h	\
	gdata-documents-folder.c	\
	gdata-documents-folder.h	\
	gdata-documents-query.c		\
	gdata-documents-query.h

libgdatadocuments_la_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_srcdir)/gdata			\
	-I$(top_srcdir)/gdata/services/documents	\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

libgdatadocuments_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatadocuments_la_LIBADD = \
	$(GDATA_LIBS)

libgdatadocuments_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

-include $(top_srcdir)/git.mk
