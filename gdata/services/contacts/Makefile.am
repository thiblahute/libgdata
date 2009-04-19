gdatacontactsincludedir = $(pkgincludedir)/gdata/services/contacts
gdatacontactsinclude_HEADERS = \
	gdata-contacts-service.h	\
	gdata-contacts-contact.h	\
	gdata-contacts-query.h

noinst_LTLIBRARIES = libgdatacontacts.la

libgdatacontacts_headers =

libgdatacontacts_la_SOURCES = \
	gdata-contacts-service.c	\
	gdata-contacts-service.h	\
	gdata-contacts-contact.c	\
	gdata-contacts-contact.h	\
	gdata-contacts-query.c		\
	gdata-contacts-query.h

libgdatacontacts_la_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_srcdir)/gdata			\
	-I$(top_srcdir)/gdata/services/contacts	\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

libgdatacontacts_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatacontacts_la_LIBADD = \
	$(GDATA_LIBS)

libgdatacontacts_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

-include $(top_srcdir)/git.mk
