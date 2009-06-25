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

#ifndef GDATA_GENERATOR_H
#define GDATA_GENERATOR_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GENERATOR		(gdata_generator_get_type ())
#define GDATA_GENERATOR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GENERATOR, GDataGenerator))
#define GDATA_GENERATOR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GENERATOR, GDataGeneratorClass))
#define GDATA_IS_GENERATOR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GENERATOR))
#define GDATA_IS_GENERATOR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GENERATOR))
#define GDATA_GENERATOR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GENERATOR, GDataGeneratorClass))

typedef struct _GDataGeneratorPrivate	GDataGeneratorPrivate;

/**
 * GDataGenerator:
 *
 * All the fields in the #GDataGenerator structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGeneratorPrivate *priv;
} GDataGenerator;

/**
 * GDataGeneratorClass:
 *
 * All the fields in the #GDataGeneratorClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGeneratorClass;

GType gdata_generator_get_type (void) G_GNUC_CONST;

gint gdata_generator_compare (const GDataGenerator *a, const GDataGenerator *b);

const gchar *gdata_generator_get_name (GDataGenerator *self);
const gchar *gdata_generator_get_uri (GDataGenerator *self);
const gchar *gdata_generator_get_version (GDataGenerator *self);

G_END_DECLS

#endif /* !GDATA_GENERATOR_H */
