SUBDIRS = atom gd media services . tests
DIST_SUBDIRS = atom gd media services tests

# Marshalling
GDATA_MARSHAL_FILES = \
	gdata-marshal.c	\
	gdata-marshal.h

GLIB_GENMARSHAL = `pkg-config --variable=glib_genmarshal glib-2.0`

gdata-marshal.h: gdata-marshal.list Makefile
	( $(GLIB_GENMARSHAL) --prefix=gdata_marshal $(srcdir)/gdata-marshal.list --header > gdata-marshal.h )
gdata-marshal.c: gdata-marshal.h Makefile
	( $(GLIB_GENMARSHAL) --prefix=gdata_marshal $(srcdir)/gdata-marshal.list --header --body > gdata-marshal.c )

# Enums
GDATA_ENUM_FILES = \
	gdata-enums.c	\
	gdata-enums.h

gdata-enums.h: $(gdata_headers) Makefile
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#ifndef GDATA_ENUMS_H\n#define GDATA_ENUMS_H\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define GDATA_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* !GDATA_ENUMS_H */" $(gdata_headers)) > gdata-enums.h.tmp \
	&& sed "s/g_data/gdata/" gdata-enums.h.tmp > gdata-enums.h \
	&& rm -f gdata-enums.h.tmp

gdata-enums.c: $(gdata_headers) Makefile gdata-enums.h
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#include \"gdata.h\"\n#include \"gdata-enums.h\"" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(gdata_headers)) > gdata-enums.c.tmp \
	&& sed "s/g_data/gdata/" gdata-enums.c.tmp > gdata-enums.c \
	&& rm -f gdata-enums.c.tmp

# libgdata library
lib_LTLIBRARIES = libgdata.la

gdata_headers = \
	gdata.h			\
	gdata-entry.h		\
	gdata-feed.h		\
	gdata-service.h		\
	gdata-query.h		\
	gdata-access-handler.h	\
	gdata-access-rule.h	\
	gdata-parsable.h

gdataincludedir = $(pkgincludedir)/gdata
gdatainclude_HEADERS = \
	$(gdata_headers)	\
	gdata-enums.h		\
	gdata-types.h

libgdata_la_SOURCES = \
	$(GDATA_MARSHAL_FILES)	\
	$(GDATA_ENUM_FILES)	\
	gdata-entry.c		\
	gdata-feed.c		\
	gdata-service.c		\
	gdata-types.c		\
	gdata-query.c		\
	gdata-parser.c		\
	gdata-access-handler.c	\
	gdata-access-rule.c	\
	gdata-parsable.c	\
	gdata-private.h		\
	gdata-parser.h

libgdata_la_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdata		\
	$(DISABLE_DEPRECATED)		\
	$(AM_CPPFLAGS)

libgdata_la_CFLAGS = \
	$(GDATA_CFLAGS)			\
	$(GNOME_CFLAGS)			\
	$(WARN_CFLAGS)			\
	$(AM_CFLAGS)			\
	-D_GNU_SOURCE

libgdata_la_LIBADD = \
	$(GDATA_LIBS)				\
	$(GNOME_LIBS)				\
	atom/libgdataatom.la			\
	gd/libgdatagd.la			\
	media/libgdatamedia.la			\
	services/youtube/libgdatayoutube.la	\
	services/calendar/libgdatacalendar.la	\
	services/picasaweb/libgdatapicasaweb.la	\
	services/documents/libgdatadocuments.la \
	services/contacts/libgdatacontacts.la

libgdata_la_LDFLAGS = \
	-version-info $(GDATA_LT_VERSION)	\
	-export-symbols $(srcdir)/gdata.symbols \
	-no-undefined				\
	$(AM_LDFLAGS)

# General cleanup
CLEANFILES = \
	$(GDATA_ENUM_FILES)	\
	$(GDATA_MARSHAL_FILES)
EXTRA_DIST = \
	gdata.symbols	\
	gdata-marshal.list

-include $(top_srcdir)/git.mk
