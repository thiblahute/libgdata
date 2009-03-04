include $(top_srcdir)/Makefile.decl

pwd=`pwd`
INCLUDES = \
	-I$(top_srcdir)/gdata		\
	-DTEST_FILE_DIR=\""$(pwd)/"\"

noinst_PROGRAMS = $(TEST_PROGS)

TEST_PROGS			+= general
general_SOURCES			 = general.c
general_LDADD			 = \
				$(progs_ldadd)		\
				../libgdata.la		\
				$(GDATA_LIBS)
general_CPPFLAGS		 = \
				$(common_defines)	\
				$(AM_CPPFLAGS)
general_CFLAGS			 = \
				$(WARN_CFLAGS)		\
				$(GDATA_CFLAGS)		\
				$(AM_CFLAGS)
general_LDFLAGS			 = $(AM_LDFLAGS)

TEST_PROGS			+= youtube
youtube_SOURCES			 = youtube.c
youtube_LDADD			 = \
				$(progs_ldadd)		\
				$(GDATA_LIBS)		\
				../libgdata.la
youtube_CPPFLAGS		 = \
				-I$(top_srcdir)			\
				-I$(top_builddir)/gdata		\
				-DGNOMELOCALEDIR=\""$(datadir)/locale"\"\
				$(DISABLE_DEPRECATED)		\
				$(AM_CPPFLAGS)
youtube_CFLAGS			 = \
				$(GDATA_CFLAGS)		\
				$(WARN_CFLAGS)		\
				$(AM_CFLAGS)

TEST_PROGS			+= calendar
calendar_SOURCES		 = calendar.c
calendar_LDADD			 = \
				$(progs_ldadd)		\
				$(GDATA_LIBS)		\
				../libgdata.la
calendar_CPPFLAGS		 = \
				-I$(top_srcdir)			\
				-I$(top_builddir)/gdata		\
				-DGNOMELOCALEDIR=\""$(datadir)/locale"\"\
				$(DISABLE_DEPRECATED)		\
				$(AM_CPPFLAGS)
calendar_CFLAGS			 = \
				$(GDATA_CFLAGS)		\
				$(WARN_CFLAGS)		\
				$(AM_CFLAGS)

TEST_PROGS			+= memory
memory_SOURCES			 = memory.c
memory_LDADD			 = \
				$(progs_ldadd)		\
				$(GDATA_LIBS)		\
				../libgdata.la
memory_CPPFLAGS			 = \
				-I$(top_srcdir)			\
				-I$(top_builddir)/gdata		\
				-DGNOMELOCALEDIR=\""$(datadir)/locale"\"\
				$(DISABLE_DEPRECATED)		\
				$(AM_CPPFLAGS)
memory_CFLAGS			 = \
				$(GDATA_CFLAGS)		\
				$(WARN_CFLAGS)		\
				$(AM_CFLAGS)
