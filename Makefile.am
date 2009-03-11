SUBDIRS = gdata po docs

EXTRA_DIST =			\
	COPYING.LIB		\
	autogen.sh		\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	AUTHORS NEWS		\
	gtk-doc.make		\
	gdata.pc.in

CLEANFILES = gdata.pc

DISTCLEANFILES = intltool-extract intltool-merge intltool-update \
		 gnome-doc-utils.make gtk-doc.make

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgdata.pc

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper --enable-gtk-doc
