# Enums
GDATA_MEDIA_ENUM_FILES = \
	gdata-media-enums.c	\
	gdata-media-enums.h

gdata-media-enums.h: $(gdata_media_headers) Makefile
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#ifndef GDATA_MEDIA_ENUMS_H\n#define GDATA_MEDIA_ENUMS_H\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define GDATA_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* !GDATA_MEDIA_ENUMS_H */" $(gdata_media_headers)) > gdata-media-enums.h.tmp \
	&& sed "s/g_data/gdata/" gdata-media-enums.h.tmp > gdata-media-enums.h.tmp2 \
	&& sed "s/GDATA_TYPE_DATA/GDATA_TYPE/" gdata-media-enums.h.tmp2 > gdata-media-enums.h \
	&& rm -f gdata-media-enums.h.tmp \
	&& rm -f gdata-media-enums.h.tmp2

gdata-media-enums.c: $(gdata_media_headers) Makefile gdata-media-enums.h
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#include \"gdata-media-content.h\"\n#include \"gdata-media-enums.h\"" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(gdata_media_headers)) > gdata-media-enums.c.tmp \
	&& sed "s/g_data/gdata/" gdata-media-enums.c.tmp > gdata-media-enums.c \
	&& rm -f gdata-media-enums.c.tmp

# Library
gdata_media_headers = \
	gdata-media-category.h	\
	gdata-media-content.h	\
	gdata-media-credit.h	\
	gdata-media-thumbnail.h

gdatamediaincludedir = $(pkgincludedir)/gdata/media
gdatamediainclude_HEADERS = \
	$(gdata_media_headers)	\
	gdata-media-enums.h

noinst_LTLIBRARIES = libgdatamedia.la

libgdatamedia_la_SOURCES = \
	gdata-media-enums.c	\
	gdata-media-category.c	\
	gdata-media-content.c	\
	gdata-media-credit.c	\
	gdata-media-thumbnail.c	\
	gdata-media-group.c	\
	gdata-media-group.h

libgdatamedia_la_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/gdata		\
	-I$(top_srcdir)/gdata/media	\
	$(DISABLE_DEPRECATED)		\
	$(AM_CPPFLAGS)

libgdatamedia_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatamedia_la_LIBADD = \
	$(GDATA_LIBS)

libgdatamedia_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

# General cleanup
CLEANFILES = $(GDATA_MEDIA_ENUM_FILES)

-include $(top_srcdir)/git.mk
