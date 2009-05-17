/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
 * 
 * GData Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GDATA_PARSABLE_H
#define GDATA_PARSABLE_H

#include <glib.h>
#include <glib-object.h>
#include <libxml/parser.h>

G_BEGIN_DECLS

#define GDATA_TYPE_PARSABLE		(gdata_parsable_get_type ())
#define GDATA_PARSABLE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_PARSABLE, GDataParsable))
#define GDATA_PARSABLE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_PARSABLE, GDataParsableClass))
#define GDATA_IS_PARSABLE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_PARSABLE))
#define GDATA_IS_PARSABLE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_PARSABLE))
#define GDATA_PARSABLE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_PARSABLE, GDataParsableClass))

typedef struct _GDataParsablePrivate	GDataParsablePrivate;

/**
 * GDataParsable:
 *
 * All the fields in the #GDataParsable structure are private and should never be accessed directly.
 *
 * Since: 0.3.0
 **/
typedef struct {
	GObject parent;
	GDataParsablePrivate *priv;
} GDataParsable;

/**
 * GDataParsableClass:
 * @parent: the parent class
 * @pre_parse_xml: a function called before parsing of an XML tree is started, which allows properties from the root node to be extracted
 * and used in @parsable
 * @parse_xml: a function to parse an XML representation of the #GDataParsable to set the properties of the @parsable
 * @post_parse_xml: a function called after parsing an XML tree, to allow the @parsable to validate the parsed properties
 *
 * The class structure for the #GDataParsable class.
 *
 * Since: 0.3.0
 **/
typedef struct {
	GObjectClass parent;

	gboolean (*pre_parse_xml) (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
	gboolean (*parse_xml) (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
	gboolean (*post_parse_xml) (GDataParsable *parsable, gpointer user_data, GError **error);
} GDataParsableClass;

GType gdata_parsable_get_type (void);

G_END_DECLS

#endif /* !GDATA_PARSABLE_H */
