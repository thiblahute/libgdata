## Process this file with automake to produce Makefile.in

# We require automake 1.6 at least.
AUTOMAKE_OPTIONS = 1.6

# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=gdata

# The top-level SGML file. You can change this if you want to.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../gtk
DOC_SOURCE_DIR=../../gdata

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED" 
SCAN_OPTIONS=--rebuild-types

# Extra options to supply to gtkdoc-mkdb.
# e.g. MKDB_OPTIONS=--sgml-mode --output-format=xml
MKDB_OPTIONS=--sgml-mode --output-format=xml --name-space=gdata

# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=

# Add the builddir to the HTML path so that version.xml is found
MKHTML_OPTIONS=--path="$(abs_builddir)"

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB=$(top_srcdir)/gdata/*.h $(top_srcdir)/gdata/services/*/*.h
CFILE_GLOB=$(top_srcdir)/gdata/*.c $(top_srcdir)/gdata/services/*/*.c

# Header files to ignore when scanning.
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES = \
	common.h		\
	gdata-private.h		\
	gdata-parser.h		\
	gdata-marshal.h		\
	gdata-enums.h		\
	gdata-media-enums.h	\
	gdata-media-group.c	\
	gdata-media-group.h	\
	gdata-youtube-group.c	\
	gdata-youtube-group.h	\
	gdata-youtube-control.c	\
	gdata-youtube-control.h	\
	gdata-picasaweb-enums.h

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES=structure.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files=version.xml gdata-overview.xml

# SGML files where gtk-doc abbrevations (#GtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files=

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. INCLUDES=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
INCLUDES=-I$(top_srcdir) -I$(top_builddir) $(GDATA_CFLAGS)
GTKDOC_LIBS=$(top_builddir)/gdata/libgdata.la

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# This should be in gtk-doc.make, since we are calling gtkdoc-scan with
# --rebuild-types
SCANOBJ_FILES += $(DOC_MODULE).types

# This should also be dealt with elsewhere, but we can't call gtkdocize
# with --flavour=no-tmpl because of gnome-autogen.sh being out of our
# control. Grrr.
distclean-local:
	cd $(srcdir) && \
	  rm -rf xml tmpl $(REPORT_FILES) \
	         $(DOC_MODULE)-decl-list.txt $(DOC_MODULE)-decl.txt

maintainer-clean-local: clean
	cd $(srcdir) && rm -rf xml tmpl html

GITIGNOREFILES = tmpl/

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST += version.xml.in structure.dia

if ENABLE_GTK_DOC
TESTS_ENVIRONMENT = cd $(srcdir) && 
TESTS = $(GTKDOC_CHECK)
endif

-include $(top_srcdir)/git.mk
