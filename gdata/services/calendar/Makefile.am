gdatacalendarincludedir = $(pkgincludedir)/gdata/services/calendar
gdatacalendarinclude_HEADERS = \
	gdata-calendar-service.h	\
	gdata-calendar-calendar.h	\
	gdata-calendar-event.h		\
	gdata-calendar-query.h		\
	gdata-calendar-feed.h

noinst_LTLIBRARIES = libgdatacalendar.la

libgdatacalendar_headers =

libgdatacalendar_la_SOURCES = \
	gdata-calendar-service.c	\
	gdata-calendar-service.h	\
	gdata-calendar-calendar.c	\
	gdata-calendar-calendar.h	\
	gdata-calendar-event.c		\
	gdata-calendar-event.h		\
	gdata-calendar-query.c		\
	gdata-calendar-query.h		\
	gdata-calendar-feed.c		\
	gdata-calendar-feed.h

libgdatacalendar_la_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_srcdir)/gdata			\
	-I$(top_srcdir)/gdata/services/calendar	\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

libgdatacalendar_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatacalendar_la_LIBADD = \
	$(GDATA_LIBS)

libgdatacalendar_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

-include $(top_srcdir)/git.mk
