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

/**
 * SECTION:gdata-gd-when
 * @short_description: GData when element
 * @stability: Unstable
 * @include: gdata/gd/gdata-gd-when.h
 *
 * #GDataGDWhen represents a "when" element from the
 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-gd-when.h"
#include "gdata-gd-reminder.h"
#include "gdata-private.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-types.h"

static void gdata_gd_when_dispose (GObject *object);
static void gdata_gd_when_finalize (GObject *object);
static void gdata_gd_when_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_gd_when_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean post_parse_xml (GDataParsable *parsable, gpointer user_data, GError **error);
static void pre_get_xml (GDataParsable *parsable, GString *xml_string);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataGDWhenPrivate {
	GTimeVal start_time;
	GTimeVal end_time;
	gboolean is_date;
	gchar *value_string;
	GList *reminders;
};

enum {
	PROP_START_TIME = 1,
	PROP_END_TIME,
	PROP_IS_DATE,
	PROP_VALUE_STRING
};

G_DEFINE_TYPE (GDataGDWhen, gdata_gd_when, GDATA_TYPE_PARSABLE)
#define GDATA_GD_WHEN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_GD_WHEN, GDataGDWhenPrivate))

static void
gdata_gd_when_class_init (GDataGDWhenClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataGDWhenPrivate));

	gobject_class->get_property = gdata_gd_when_get_property;
	gobject_class->set_property = gdata_gd_when_set_property;
	gobject_class->dispose = gdata_gd_when_dispose;
	gobject_class->finalize = gdata_gd_when_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->post_parse_xml = post_parse_xml;
	parsable_class->pre_get_xml = pre_get_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataGDWhen:start-time:
	 *
	 * The name of the when.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_START_TIME,
				g_param_spec_boxed ("start-time",
					"Start time", "The name of the when.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWhen:end-time:
	 *
	 * The title of a person within the when.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_END_TIME,
				g_param_spec_boxed ("end-time",
					"End time", "The title of a person within the when.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWhen:is-date:
	 *
	 * A programmatic value that identifies the type of when.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_DATE,
				g_param_spec_boolean ("is-date",
					"Date?", "A programmatic value that identifies the type of when.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWhen:value-string:
	 *
	 * A simple string value used to name this when. It allows UIs to display a label such as "Work", "Volunteer",
	 * "Professional Society", etc.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_VALUE_STRING,
				g_param_spec_string ("value-string",
					"Value string", "A simple string value used to name this when.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_gd_when_init (GDataGDWhen *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_GD_WHEN, GDataGDWhenPrivate);
}

static void
gdata_gd_when_dispose (GObject *object)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (object)->priv;

	if (priv->reminders != NULL) {
		g_list_foreach (priv->reminders, (GFunc) g_object_unref, NULL);
		g_list_free (priv->reminders);
	}
	priv->reminders = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_gd_when_parent_class)->dispose (object);
}

static void
gdata_gd_when_finalize (GObject *object)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (object)->priv;

	g_free (priv->value_string);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_gd_when_parent_class)->finalize (object);
}

static void
gdata_gd_when_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (object)->priv;

	switch (property_id) {
		case PROP_START_TIME:
			g_value_set_boxed (value, &(priv->start_time));
			break;
		case PROP_END_TIME:
			g_value_set_boxed (value, &(priv->end_time));
			break;
		case PROP_IS_DATE:
			g_value_set_boolean (value, priv->is_date);
			break;
		case PROP_VALUE_STRING:
			g_value_set_string (value, priv->value_string);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_gd_when_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataGDWhen *self = GDATA_GD_WHEN (object);

	switch (property_id) {
		case PROP_START_TIME:
			gdata_gd_when_set_start_time (self, g_value_get_boxed (value));
			break;
		case PROP_END_TIME:
			gdata_gd_when_set_end_time (self, g_value_get_boxed (value));
			break;
		case PROP_IS_DATE:
			gdata_gd_when_set_is_date (self, g_value_get_boolean (value));
			break;
		case PROP_VALUE_STRING:
			gdata_gd_when_set_value_string (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (parsable)->priv;
	xmlChar *start_time, *end_time, *value_string;
	GTimeVal start_time_timeval, end_time_timeval;
	gboolean is_date = FALSE;

	/* Start time */
	start_time = xmlGetProp (root_node, (xmlChar*) "startTime");
	if (gdata_parser_time_val_from_date ((gchar*) start_time, &start_time_timeval) == TRUE) {
		is_date = TRUE;
	} else if (g_time_val_from_iso8601 ((gchar*) start_time, &start_time_timeval) == FALSE) {
		/* Error */
		gdata_parser_error_not_iso8601_format (root_node, (gchar*) start_time, error);
		xmlFree (start_time);
		return FALSE;
	}
	xmlFree (start_time);

	/* End time (optional) */
	end_time = xmlGetProp (root_node, (xmlChar*) "endTime");
	if (end_time != NULL) {
		gboolean success;

		if (is_date == TRUE)
			success = gdata_parser_time_val_from_date ((gchar*) end_time, &end_time_timeval);
		else
			success = g_time_val_from_iso8601 ((gchar*) end_time, &end_time_timeval);

		if (success == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (root_node, (gchar*) end_time, error);
			xmlFree (end_time);
			return FALSE;
		}
		xmlFree (end_time);
	} else {
		/* Give a default */
		end_time_timeval.tv_sec = end_time_timeval.tv_usec = 0;
	}

	value_string = xmlGetProp (root_node, (xmlChar*) "value");

	priv->start_time = start_time_timeval;
	priv->end_time = end_time_timeval;
	priv->is_date = is_date;
	priv->value_string = g_strdup ((gchar*) value_string);

	xmlFree (value_string);

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (parsable)->priv;

	if (xmlStrcmp (node->name, (xmlChar*) "reminder") == 0) {
		/* gd:reminder */
		GDataGDReminder *reminder = GDATA_GD_REMINDER (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_REMINDER, "reminder", doc, node,
												  NULL, error));
		if (reminder == NULL)
			return FALSE;

		priv->reminders = g_list_prepend (priv->reminders, reminder);
	} else if (GDATA_PARSABLE_CLASS (gdata_gd_when_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static gboolean
post_parse_xml (GDataParsable *parsable, gpointer user_data, GError **error)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (parsable)->priv;

	/* Reverse our lists of stuff */
	priv->reminders = g_list_reverse (priv->reminders);

	return TRUE;
}

static void
pre_get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (parsable)->priv;
	gchar *start_time;

	if (priv->is_date == TRUE)
		start_time = gdata_parser_date_from_time_val (&(priv->start_time));
	else
		start_time = g_time_val_to_iso8601 (&(priv->start_time));

	g_string_append_printf (xml_string, " startTime='%s'", start_time);
	g_free (start_time);

	if (priv->end_time.tv_sec != 0 || priv->end_time.tv_usec != 0) {
		gchar *end_time;

		if (priv->is_date == TRUE)
			end_time = gdata_parser_date_from_time_val (&(priv->end_time));
		else
			end_time = g_time_val_to_iso8601 (&(priv->end_time));

		g_string_append_printf (xml_string, " endTime='%s'", end_time);
		g_free (end_time);
	}

	if (priv->value_string != NULL)
		g_string_append_printf (xml_string, " value='%s'", priv->value_string);
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GList *reminders;
	GDataGDWhenPrivate *priv = GDATA_GD_WHEN (parsable)->priv;

	for (reminders = priv->reminders; reminders != NULL; reminders = reminders->next)
		g_string_append (xml_string, _gdata_parsable_get_xml (GDATA_PARSABLE (reminders->data), "gd:reminder", FALSE));
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
}

/**
 * gdata_gd_when_new:
 * @start_time: when the event starts or (for zero-duration events) when it occurs
 * @end_time: when the event ends, or %NULL
 * @is_date: %TRUE if @start_time and @end_time specify dates rather than times, %FALSE otherwise
 *
 * Creates a new #GDataGDWhen. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhen">GData specification</ulink>.
 *
 * Return value: a new #GDataGDWhen, or %NULL; unref with g_object_unref()
 *
 * Since: 0.2.0
 **/
GDataGDWhen *
gdata_gd_when_new (GTimeVal *start_time, GTimeVal *end_time, gboolean is_date)
{
	g_return_val_if_fail (start_time != NULL, NULL);
	return g_object_new (GDATA_TYPE_GD_WHEN, "start-time", start_time, "end-time", end_time, "is-date", is_date, NULL);
}

/**
 * gdata_gd_when_compare:
 * @a: a #GDataGDWhen, or %NULL
 * @b: another #GDataGDWhen, or %NULL
 *
 * Compares the two times in a strcmp() fashion. %NULL values are handled gracefully, with
 * %0 returned if both @a and @b are %NULL, %-1 if @a is %NULL and %1 if @b is %NULL.
 *
 * The comparison of non-%NULL values is done on the basis of the @start_time, @end_time and @is_date properties of the #GDataGDWhen<!-- -->s.
 *
 * Return value: %0 if @a equals @b, %-1 or %1 as appropriate otherwise
 *
 * Since: 0.4.0
 **/
gint
gdata_gd_when_compare (const GDataGDWhen *a, const GDataGDWhen *b)
{
	if (a == NULL && b != NULL)
		return -1;
	else if (b == NULL)
		return 1;

	if (a == b)
		return 0;
	if (a->priv->is_date != b->priv->is_date)
		return CLAMP (b->priv->is_date - a->priv->is_date, -1, 1);

	if (a->priv->start_time.tv_sec == b->priv->start_time.tv_sec && a->priv->start_time.tv_usec == b->priv->start_time.tv_usec)
		return CLAMP ((b->priv->end_time.tv_sec * 1000 + b->priv->end_time.tv_usec) -
			      (a->priv->end_time.tv_sec * 1000 + a->priv->end_time.tv_usec), -1, 1);
	return CLAMP ((b->priv->start_time.tv_sec * 1000 + b->priv->start_time.tv_usec) -
		      (a->priv->start_time.tv_sec * 1000 + a->priv->start_time.tv_usec), -1, 1);
}

/**
 * gdata_gd_when_get_start_time:
 * @self: a #GDataGDWhen
 * @start_time: return location for the start time
 *
 * Gets the #GDataGDWhen:start-time property and returns it in @start_time.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_get_start_time (GDataGDWhen *self, GTimeVal *start_time)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));
	g_return_if_fail (start_time != NULL);
	*start_time = self->priv->start_time;
}

/**
 * gdata_gd_when_set_start_time:
 * @self: a #GDataGDWhen
 * @start_time: the new start time
 *
 * Sets the #GDataGDWhen:start-time property to @start_time.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_set_start_time (GDataGDWhen *self, GTimeVal *start_time)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));
	g_return_if_fail (start_time != NULL);

	self->priv->start_time = *start_time;
	g_object_notify (G_OBJECT (self), "start-time");
}

/**
 * gdata_gd_when_get_end_time:
 * @self: a #GDataGDWhen
 * @end_time: return location for the end time
 *
 * Gets the #GDataGDWhen:end-time property and returns it in @end_time.
 *
 * If the end time is unset, both fields of the #GTimeVal will be %0.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_get_end_time (GDataGDWhen *self, GTimeVal *end_time)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));
	g_return_if_fail (end_time != NULL);
	*end_time = self->priv->end_time;
}

/**
 * gdata_gd_when_set_end_time:
 * @self: a #GDataGDWhen
 * @end_time: the new end time, or %NULL
 *
 * Sets the #GDataGDWhen:end-time property to @end_time.
 *
 * Set @end_time to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_set_end_time (GDataGDWhen *self, GTimeVal *end_time)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));

	if (end_time != NULL)
		self->priv->end_time = *end_time;
	else
		self->priv->end_time.tv_sec = self->priv->end_time.tv_usec = 0;

	g_object_notify (G_OBJECT (self), "end-time");
}

/**
 * gdata_gd_when_is_date:
 * @self: a #GDataGDWhen
 *
 * Gets the #GDataGDWhen:is-date property.
 *
 * Return value: %TRUE if #GDataGDWhen:start-time and #GDataGDWhen:end-time are dates rather than times, %FALSE otherwise
 *
 * Since: 0.4.0
 **/
gboolean
gdata_gd_when_is_date (GDataGDWhen *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHEN (self), FALSE);
	return self->priv->is_date;
}

/**
 * gdata_gd_when_set_is_date:
 * @self: a #GDataGDWhen
 * @is_date: %TRUE if #GDataGDWhen:start-time and #GDataGDWhen:end-time should be dates rather than times, %FALSE otherwise
 *
 * Sets the #GDataGDWhen:is-date property to @is_date.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_set_is_date (GDataGDWhen *self, gboolean is_date)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));

	self->priv->is_date = is_date;
	g_object_notify (G_OBJECT (self), "is-date");
}

/**
 * gdata_gd_when_get_value_string:
 * @self: a #GDataGDWhen
 *
 * Gets the #GDataGDWhen:value-string property.
 *
 * Return value: the value string, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_when_get_value_string (GDataGDWhen *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHEN (self), NULL);
	return self->priv->value_string;
}

/**
 * gdata_gd_when_set_value_string:
 * @self: a #GDataGDWhen
 * @value_string: the new value string, or %NULL
 *
 * Sets the #GDataGDWhen:value-string property to @value_string.
 *
 * Set @value_string to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_when_set_value_string (GDataGDWhen *self, const gchar *value_string)
{
	g_return_if_fail (GDATA_IS_GD_WHEN (self));

	g_free (self->priv->value_string);
	self->priv->value_string = g_strdup (value_string);
	g_object_notify (G_OBJECT (self), "value-string");
}

/**
 * gdata_gd_when_get_reminders:
 * @self: a #GDataGDWhen
 *
 * Returns a list of the #GDataGDReminder<!-- -->s which are associated with this #GDataGDWhen.
 *
 * Return value: a #GList of #GDataGDReminder<!-- -->s, or %NULL
 *
 * Since: 0.4.0
 **/
GList *
gdata_gd_when_get_reminders (GDataGDWhen *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHEN (self), NULL);
	return self->priv->reminders;
}
