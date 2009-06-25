gdatagdincludedir = $(pkgincludedir)/gdata/gd
gdatagdinclude_HEADERS = \
	gdata-gd-email-address.h	\
	gdata-gd-im-address.h		\
	gdata-gd-organization.h		\
	gdata-gd-phone-number.h		\
	gdata-gd-postal-address.h	\
	gdata-gd-reminder.h		\
	gdata-gd-when.h			\
	gdata-gd-where.h		\
	gdata-gd-who.h

noinst_LTLIBRARIES = libgdatagd.la

libgdatagd_la_SOURCES = \
	gdata-gd-email-address.c	\
	gdata-gd-im-address.c		\
	gdata-gd-organization.c		\
	gdata-gd-phone-number.c		\
	gdata-gd-postal-address.c	\
	gdata-gd-reminder.c		\
	gdata-gd-when.c			\
	gdata-gd-where.c		\
	gdata-gd-who.c

libgdatagd_la_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdata		\
	-I$(top_srcdir)/gdata/gd	\
	$(DISABLE_DEPRECATED)		\
	$(AM_CPPFLAGS)

libgdatagd_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatagd_la_LIBADD = \
	$(GDATA_LIBS)

libgdatagd_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

-include $(top_srcdir)/git.mk
