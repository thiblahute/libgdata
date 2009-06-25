# Enums
GDATA_YOUTUBE_ENUM_FILES = \
	gdata-youtube-enums.c	\
	gdata-youtube-enums.h

gdata-youtube-enums.h: $(gdatayoutubeinclude_HEADERS) Makefile
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#ifndef GDATA_YOUTUBE_ENUMS_H\n#define GDATA_YOUTUBE_ENUMS_H\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define GDATA_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* !GDATA_YOUTUBE_ENUMS_H */" $(gdatayoutubeinclude_HEADERS)) > gdata-youtube-enums.h.tmp \
	&& sed "s/g_data_you_tube/gdata_youtube/" gdata-youtube-enums.h.tmp > gdata-youtube-enums.h.tmp2 \
	&& sed "s/GDATA_TYPE_DATA_YOU_TUBE/GDATA_TYPE_YOUTUBE/" gdata-youtube-enums.h.tmp2 > gdata-youtube-enums.h \
	&& rm -f gdata-youtube-enums.h.tmp \
	&& rm -f gdata-youtube-enums.h.tmp2

gdata-youtube-enums.c: $(gdatayoutubeinclude_HEADERS) Makefile gdata-youtube-enums.h
	(cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#include \"gdata-youtube-service.h\"\n#include \"gdata-youtube-query.h\"\n#include \"gdata-youtube-enums.h\"" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(gdatayoutubeinclude_HEADERS)) > gdata-youtube-enums.c.tmp \
	&& sed "s/g_data_you_tube/gdata_youtube/" gdata-youtube-enums.c.tmp > gdata-youtube-enums.c \
	&& rm -f gdata-youtube-enums.c.tmp

# Library
gdatayoutubeincludedir = $(pkgincludedir)/gdata/services/youtube
gdatayoutubeinclude_HEADERS = \
	gdata-youtube-service.h		\
	gdata-youtube-video.h		\
	gdata-youtube-content.h		\
	gdata-youtube-credit.h		\
	gdata-youtube-enums.h		\
	gdata-youtube-query.h		\
	gdata-youtube-state.h

noinst_LTLIBRARIES = libgdatayoutube.la

libgdatayoutube_la_SOURCES = \
	gdata-youtube-enums.c		\
	gdata-youtube-service.c		\
	gdata-youtube-video.c		\
	gdata-youtube-content.c		\
	gdata-youtube-credit.c		\
	gdata-youtube-query.c		\
	gdata-youtube-group.c		\
	gdata-youtube-group.h		\
	gdata-youtube-state.c		\
	gdata-youtube-control.c		\
	gdata-youtube-control.h

libgdatayoutube_la_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_srcdir)/gdata			\
	-I$(top_srcdir)/gdata/services/youtube	\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

libgdatayoutube_la_CFLAGS = \
	$(GDATA_CFLAGS)	\
	$(WARN_CFLAGS)	\
	$(AM_CFLAGS)	\
	-D_GNU_SOURCE

libgdatayoutube_la_LIBADD = \
	$(GDATA_LIBS)

libgdatayoutube_la_LDFLAGS = \
	-no-undefined	\
	$(AM_LDFLAGS)

# General cleanup
CLEANFILES = $(GDATA_YOUTUBE_ENUM_FILES)

-include $(top_srcdir)/git.mk
