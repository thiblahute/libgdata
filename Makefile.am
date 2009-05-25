SUBDIRS = gdata po docs

EXTRA_DIST = \
	COPYING.LIB		\
	autogen.sh		\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	AUTHORS 		\
	NEWS			\
	HACKING			\
	gtk-doc.make		\
	libgdata.pc.in		\
	libgdata.doap

CLEANFILES = gdata.pc

DISTCLEANFILES = \
	intltool-extract	\
	intltool-merge		\
	intltool-update		\
	gnome-doc-utils.make	\
	gtk-doc.make

MAINTAINERCLEANFILES = \
	INSTALL		\
	aclocal.m4	\
	compile		\
	config.guess	\
	config.h.in	\
	config.sub	\
	depcomp		\
	install-sh	\
	ltmain.sh	\
	missing		\
	mkinstalldirs	\
	omf.make	\
	gtk-doc.make	\
	ChangeLog	\
	`find "$(srcdir)" -type f -name Makefile.in -print`

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgdata.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

# ChangeLog
ChangeLog: $(srcdir)/ChangeLog
$(srcdir)/ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run \
	   git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi
.PHONY: $(srcdir)/ChangeLog


-include $(top_srcdir)/git.mk
