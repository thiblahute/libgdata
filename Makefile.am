SUBDIRS = gdata po docs

EXTRA_DIST = \
	COPYING.LIB		\
	autogen.sh		\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	AUTHORS NEWS		\
	gtk-doc.make		\
	libgdata.pc.in

CLEANFILES = gdata.pc

DISTCLEANFILES = \
	intltool-extract	\
	intltool-merge		\
	intltool-update		\
	gnome-doc-utils.make	\
	gtk-doc.make

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgdata.pc

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper --enable-gtk-doc

# ChangeLog
ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi
.PHONY: ChangeLog
