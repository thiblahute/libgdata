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

/**
 * SECTION:gdata-types
 * @short_description: miscellaneous data types
 * @stability: Unstable
 * @include: gdata/gdata-types.h
 *
 * The structures here are used haphazardly across the library, describing
 * various small data types.
 **/

#include <glib.h>
#include <glib-object.h>
#include <string.h>
#include <stdlib.h>
#include <libsoup/soup.h>

#include "gdata-types.h"

static gpointer
gdata_g_time_val_copy (gpointer time_val)
{
	return g_memdup (time_val, sizeof (GTimeVal));
}

GType
gdata_g_time_val_get_type (void)
{
	static GType type_id = 0;

	if (type_id == 0) {
		type_id = g_boxed_type_register_static (g_intern_static_string ("GTimeVal"),
							(GBoxedCopyFunc) gdata_g_time_val_copy,
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

/**
 * gdata_color_from_hexadecimal:
 * @hexadecimal: a hexadecimal color string
 * @color: a #GDataColor
 *
 * Parses @hexadecimal and returns a #GDataColor describing it in @color.
 *
 * @hexadecimal should be in the form <literal>#<replaceable>rr</replaceable><replaceable>gg</replaceable><replaceable>bb</replaceable></literal>,
 * where <replaceable>rr</replaceable> is a two-digit hexadecimal red intensity value, <replaceable>gg</replaceable> is green
 * and <replaceable>bb</replaceable> is blue. The hash is optional.
 *
 * Return value: %TRUE on success, %FALSE otherwise
 **/
gboolean
gdata_color_from_hexadecimal (const gchar *hexadecimal, GDataColor *color)
{
	gchar *hex;

	g_return_val_if_fail (hexadecimal != NULL, FALSE);
	g_return_val_if_fail (color != NULL, FALSE);

	hex = g_strdup (hexadecimal);
	if (*hex == '#')
		hex++;
	if (strlen (hex) != 6) {
		g_free (hex);
		return FALSE;
	}

	/* This is horrible and hacky, but should work */
	color->blue = strtoul (hex + 4, NULL, 16);
	*(hex + 4) = '\0';
	color->green = strtoul (hex + 2, NULL, 16);
	*(hex + 2) = '\0';
	color->red = strtoul (hex, NULL, 16);

	g_free (hex);

	return TRUE;
}
