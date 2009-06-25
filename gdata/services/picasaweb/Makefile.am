# Enums
GDATA_PICASAWEB_ENUM_FILES = \
	gdata-picasaweb-enums.c	\
	gdata-picasaweb-enums.h

gdata-picasaweb-enums.h: $(gdatapicasawebinclude_HEADERS) Makefile
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#ifndef GDATA_PICASAWEB_ENUMS_H\n#define GDATA_PICASAWEB_ENUMS_H\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define GDATA_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* !GDATA_PICASAWEB_ENUMS_H */" $(gdatapicasawebinclude_HEADERS)) > gdata-picasaweb-enums.h.tmp \
	&& sed "s/g_data_picasa_web/gdata_picasaweb/" gdata-picasaweb-enums.h.tmp > gdata-picasaweb-enums.h.tmp2 \
	&& sed "s/GDATA_TYPE_DATA_PICASA_WEB/GDATA_TYPE_PICASAWEB/" gdata-picasaweb-enums.h.tmp2 > gdata-picasaweb-enums.h \
	&& rm -f gdata-picasaweb-enums.h.tmp \
	&& rm -f gdata-picasaweb-enums.h.tmp2

gdata-picasaweb-enums.c: $(gdatapicasawebinclude_HEADERS) Makefile gdata-picasaweb-enums.h
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#include \"gdata-picasaweb-album.h\"\n#include \"gdata-picasaweb-enums.h\"" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(gdatapicasawebinclude_HEADERS)) > gdata-picasaweb-enums.c.tmp \
	&& sed "s/g_data_picasa_web/gdata_picasaweb/" gdata-picasaweb-enums.c.tmp > gdata-picasaweb-enums.c \
	&& rm -f gdata-picasaweb-enums.c.tmp

# Library
gdatapicasawebincludedir = $(pkgincludedir)/gdata/services/picasaweb
gdatapicasawebinclude_HEADERS = \
	gdata-picasaweb-service.h	\
	gdata-picasaweb-query.h		\
	gdata-picasaweb-file.h		\
	gdata-picasaweb-album.h		\
	gdata-picasaweb-enums.h

noinst_LTLIBRARIES = libgdatapicasaweb.la

libgdatapicasaweb_la_SOURCES = \
	gdata-picasaweb-enums.c		\
	gdata-picasaweb-album.c		\
	gdata-picasaweb-query.c		\
	gdata-picasaweb-file.c		\
	gdata-picasaweb-service.c

libgdatapicasaweb_la_CPPFLAGS = \
	-I$(top_srcdir)					\
	-I$(top_srcdir)/gdata				\
	-I$(top_srcdir)/gdata/services/picasaweb	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)

libgdatapicasaweb_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatapicasaweb_la_LIBADD = \
	$(GDATA_LIBS)

libgdatapicasaweb_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

# General cleanup
CLEANFILES = $(GDATA_PICASAWEB_ENUM_FILES)

-include $(top_srcdir)/git.mk
