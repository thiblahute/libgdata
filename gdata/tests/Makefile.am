include $(top_srcdir)/Makefile.decl

pwd=`pwd`
INCLUDES = \
	-I$(top_srcdir)/			\
	-I$(top_srcdir)/gdata			\
	-DTEST_FILE_DIR=\""$(pwd)/"\"		\
	$(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)				\
	$(GDATA_CFLAGS)

LIBS = \
	$(top_builddir)/gdata/libgdata.la	\
	$(GDATA_LIBS)

noinst_PROGRAMS = $(TEST_PROGS)

TEST_SRCS = common.h

TEST_PROGS			+= general
general_SOURCES			 = general.c $(TEST_SRCS)

TEST_PROGS			+= youtube
youtube_SOURCES			 = youtube.c $(TEST_SRCS)

TEST_PROGS			+= calendar
calendar_SOURCES		 = calendar.c $(TEST_SRCS)

TEST_PROGS			+= contacts
contacts_SOURCES		 = contacts.c $(TEST_SRCS)

TEST_PROGS			+= picasaweb
picasaweb_SOURCES		 = picasaweb.c $(TEST_SRCS)

TEST_PROGS			+= documents
documents_SOURCES		 = documents.c $(TEST_SRCS)

TEST_PROGS			+= memory
memory_SOURCES			 = memory.c $(TEST_SRCS)

-include $(top_srcdir)/git.mk
