/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
 *
 * GData Client is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <glib-object.h>
#include <string.h>
#include <stdlib.h>

#include "gdata-types.h"

static gpointer
g_time_val_copy (gpointer time_val)
{
	return g_memdup (time_val, sizeof (GTimeVal));
}

GType
g_time_val_get_type (void)
{
	static GType type_id = 0;

	if (type_id == 0) {
		type_id = g_boxed_type_register_static (g_intern_static_string ("GTimeVal"),
							(GBoxedCopyFunc) g_time_val_copy,
							(GBoxedFreeFunc) g_free);
	}

	return type_id;
}

static gpointer
gdata_color_copy (gpointer color)
{
	return g_memdup (color, sizeof (GDataColor));
}

GType
gdata_color_get_type (void)
{
	static GType type_id = 0;

	if (type_id == 0) {
		type_id = g_boxed_type_register_static (g_intern_static_string ("GDataColor"),
							(GBoxedCopyFunc) gdata_color_copy,
							(GBoxedFreeFunc) g_free);
	}

	return type_id;
}

gboolean
gdata_color_from_hexadecimal (const gchar *hexadecimal, GDataColor *color)
{
	gchar *hex = (gchar*) hexadecimal;

	if (*hex == '#')
		hex++;
	if (strlen (hex) != 6)
		return FALSE;

	/* This is horrible and hacky, but should work */
	color->blue = strtoul (hex + 4, NULL, 16);
	*(hex + 4) = '\0';
	color->green = strtoul (hex + 2, NULL, 16);
	*(hex + 2) = '\0';
	color->red = strtoul (hex, NULL, 16);

	return TRUE;
}
