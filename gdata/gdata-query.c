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

#include <glib.h>

#include "gdata-query.h"
#include "gdata-service.h"
#include "gdata-private.h"

static void gdata_query_dispose (GObject *object);
static void gdata_query_finalize (GObject *object);
static void gdata_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataQueryPrivate {
	GDataService *service;
	GCancellable *cancellable;

	guint parameter_mask;

	/* Standard query parameters (see: http://code.google.com/apis/gdata/docs/2.0/reference.html#Queries) */
	gchar *q;
	gchar *categories;
	gchar *author;
	gchar *updated_min; /* TODO: better type for these four? */
	gchar *updated_max;
	gchar *published_min;
	gchar *published_max;
	gint start_index;
	gboolean strict;
	gint max_results;
	gchar *entry_id;
};

/* TODO: actually turn these into parameters */
enum {
	PROP_SERVICE = 1,
	PROP_CANCELLED,
	PROP_Q,
	PROP_CATEGORIES,
	PROP_AUTHOR,
	PROP_UPDATED_MIN,
	PROP_UPDATED_MAX,
	PROP_PUBLISHED_MIN,
	PROP_PUBLISHED_MAX,
	PROP_START_INDEX,
	PROP_STRICT,
	PROP_MAX_RESULTS,
	PROP_ENTRY_ID
};

G_DEFINE_TYPE (GDataQuery, gdata_query, G_TYPE_OBJECT)
#define GDATA_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_QUERY, GDataQueryPrivate))

static void
gdata_query_class_init (GDataQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataQueryPrivate));

	gobject_class->set_property = gdata_query_set_property;
	gobject_class->get_property = gdata_query_get_property;
	gobject_class->dispose = gdata_query_dispose;
	gobject_class->finalize = gdata_query_finalize;

	/* TODO: Document these much better */
	g_object_class_install_property (gobject_class, PROP_SERVICE,
				g_param_spec_object ("service",
					"Service", "The service for which this query was created.",
					GDATA_TYPE_SERVICE,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CANCELLED,
				g_param_spec_boolean ("cancelled",
					"Cancelled?", "Has this query been cancelled?",
					FALSE,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_Q,
				g_param_spec_string ("q",
					"Query terms", "Query terms for which to search.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CATEGORIES,
				g_param_spec_string ("categories",
					"Category string", "Category search string.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_AUTHOR,
				g_param_spec_string ("author",
					"Author", "Author search string.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED_MIN,
				g_param_spec_string ("updated-min",
					"Minimum update date", "Minimum date for updates on returned entries.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED_MAX,
				g_param_spec_string ("updated-max",
					"Maximum update date", "Maximum date for updates on returned entries.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PUBLISHED_MIN,
				g_param_spec_string ("published-min",
					"Minimum publish date", "Minimum date for returned entries to be published.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PUBLISHED_MAX,
				g_param_spec_string ("published-max",
					"Maximum publish date", "Maximum date for returned entries to be published.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_START_INDEX,
				g_param_spec_int ("start-index",
					"Start index", "Zero-based result start index.",
					-1, G_MAXINT, -1,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_STRICT,
				g_param_spec_boolean ("strict",
					"Strict?", "Should the server be strict about the query?",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_MAX_RESULTS,
				g_param_spec_int ("max-results",
					"Maximum number of results", "The maximum number of entries to return.",
					-1, G_MAXINT, -1,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS))
;
	g_object_class_install_property (gobject_class, PROP_ENTRY_ID,
				g_param_spec_string ("entry-id",
					"Entry ID", "A specific entry ID to return.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_query_init (GDataQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_QUERY, GDataQueryPrivate);
	self->priv->start_index = -1;
	self->priv->max_results = -1;
}

static void
gdata_query_dispose (GObject *object)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	if (priv->service != NULL)
		g_object_unref (priv->service);
	priv->service = NULL;

	if (priv->cancellable != NULL)
		g_object_unref (priv->cancellable); /* TODO: should I really be holding a reference to the GCancellable? */
	priv->cancellable = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_query_parent_class)->dispose (object);
}

static void
gdata_query_finalize (GObject *object)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	g_free (priv->q);
	g_free (priv->categories);
	g_free (priv->author);
	g_free (priv->updated_min);
	g_free (priv->updated_max);
	g_free (priv->published_min);
	g_free (priv->published_max);
	g_free (priv->entry_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_query_parent_class)->finalize (object);
}

static void
gdata_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_SERVICE:
			g_value_set_object (value, priv->service);
			break;
		case PROP_CANCELLED:
			g_value_set_boolean (value, gdata_query_is_cancelled (GDATA_QUERY (object)));
			break;
		case PROP_Q:
			g_value_set_string (value, priv->q);
			break;
		case PROP_CATEGORIES:
			g_value_set_string (value, priv->categories);
			break;
		case PROP_AUTHOR:
			g_value_set_string (value, priv->author);
			break;
		case PROP_UPDATED_MIN:
			g_value_set_string (value, priv->updated_min);
			break;
		case PROP_UPDATED_MAX:
			g_value_set_string (value, priv->updated_max);
			break;
		case PROP_PUBLISHED_MIN:
			g_value_set_string (value, priv->published_min);
			break;
		case PROP_PUBLISHED_MAX:
			g_value_set_string (value, priv->published_max);
			break;
		case PROP_START_INDEX:
			g_value_set_int (value, priv->start_index);
			break;
		case PROP_STRICT:
			g_value_set_boolean (value, priv->strict);
			break;
		case PROP_MAX_RESULTS:
			g_value_set_int (value, priv->max_results);
			break;
		case PROP_ENTRY_ID:
			g_value_set_string (value, priv->entry_id);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_SERVICE:
			priv->service = g_value_dup_object (value);
			break;
		case PROP_Q:
			gdata_query_set_q (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_CATEGORIES:
			gdata_query_set_categories (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_AUTHOR:
			gdata_query_set_author (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_UPDATED_MIN:
			gdata_query_set_updated_min (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_UPDATED_MAX:
			gdata_query_set_updated_max (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_PUBLISHED_MIN:
			gdata_query_set_published_min (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_PUBLISHED_MAX:
			gdata_query_set_published_max (GDATA_QUERY (object), g_value_get_string (value));
			break;
		case PROP_START_INDEX:
			gdata_query_set_start_index (GDATA_QUERY (object), g_value_get_int (value));
			break;
		case PROP_STRICT:
			gdata_query_set_strict (GDATA_QUERY (object), g_value_get_boolean (value));
			break;
		case PROP_MAX_RESULTS:
			gdata_query_set_max_results (GDATA_QUERY (object), g_value_get_int (value));
			break;
		case PROP_ENTRY_ID:
			gdata_query_set_entry_id (GDATA_QUERY (object), g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataQuery *
gdata_query_new (GDataService *service, const gchar *q)
{
	return g_object_new (GDATA_TYPE_QUERY,
			     "service", service,
			     "q", q,
			     NULL);
}

GDataQuery *
gdata_query_new_for_id (GDataService *service, const gchar *entry_id)
{
	return g_object_new (GDATA_TYPE_QUERY,
			     "service", service,
			     "entry-id", entry_id,
			     NULL);
}

gchar *
gdata_query_get_query_uri (GDataQuery *self, const gchar *feed_uri)
{
	GDataQueryPrivate *priv = self->priv;
	GString *query_uri;
	gboolean params_started = FALSE;

	#define APPEND_SEP g_string_append_c (query_uri, (params_started == FALSE) ? '?' : '&'); params_started = TRUE;

	/* Check to see if any parameters have been set */
	if ((priv->parameter_mask & GDATA_QUERY_PARAM_ALL) == 0)
		return g_strdup (feed_uri);

	query_uri = g_string_new (feed_uri);

	/* If we've been provided with an entry ID, only append that */
	if (priv->entry_id != NULL) {
		g_string_append_c (query_uri, '/');
		g_string_append_uri_escaped (query_uri, priv->entry_id, NULL, TRUE);
		return g_string_free (query_uri, FALSE);
	}

	if (priv->categories != NULL) {
		g_string_append (query_uri, "/-/");
		g_string_append_uri_escaped (query_uri, priv->categories, "/", TRUE);
	}

	/* If that's it, return */
	if ((priv->parameter_mask & (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_ENTRY_ID ^ GDATA_QUERY_PARAM_CATEGORIES)) == 0)
		return g_string_free (query_uri, FALSE);

	/* q param */
	if (priv->q != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "q=");
		g_string_append_uri_escaped (query_uri, priv->q, NULL, TRUE);
	}

	if (priv->author != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "author=");
		g_string_append_uri_escaped (query_uri, priv->author, NULL, TRUE);
	}

	if (priv->updated_min != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "updated-min=");
		g_string_append_uri_escaped (query_uri, priv->updated_max, NULL, TRUE);
	}

	if (priv->updated_max != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "updated-max=");
		g_string_append_uri_escaped (query_uri, priv->updated_max, NULL, TRUE);
	}

	if (priv->published_min != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "published-min=");
		g_string_append_uri_escaped (query_uri, priv->published_min, NULL, TRUE);
	}

	if (priv->published_max != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "published-max=");
		g_string_append_uri_escaped (query_uri, priv->published_max, NULL, TRUE);
	}

	if (priv->start_index > 0) {
		APPEND_SEP
		g_string_append_printf (query_uri, "start-index=%d", priv->start_index);
	}

	if (priv->strict == TRUE) {
		APPEND_SEP
		g_string_append (query_uri, "strict=true");
	}

	if (priv->max_results > 0) {
		APPEND_SEP
		g_string_append_printf (query_uri, "max-results=%d", priv->max_results);
	}

	return g_string_free (query_uri, FALSE);
}

GDataService *
gdata_query_get_service (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->service;
}

gboolean
gdata_query_is_cancelled (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), FALSE);
	if (self->priv->cancellable == NULL)
		return FALSE;
	return g_cancellable_is_cancelled (self->priv->cancellable);
}

const gchar *
gdata_query_get_q (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->q;
}

void
gdata_query_set_q (GDataQuery *self, const gchar *q)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->q);
	self->priv->q = g_strdup (q);

	if (q == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_Q);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_Q;

	g_object_notify (G_OBJECT (self), "q");
}

const gchar *
gdata_query_get_categories (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->categories;
}

void
gdata_query_set_categories (GDataQuery *self, const gchar *categories)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->categories);
	self->priv->categories = g_strdup (categories);

	if (categories == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_CATEGORIES);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_CATEGORIES;

	g_object_notify (G_OBJECT (self), "categories");
}

const gchar *
gdata_query_get_author (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->author;
}

void
gdata_query_set_author (GDataQuery *self, const gchar *author)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->author);
	self->priv->author = g_strdup (author);

	if (author == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_AUTHOR);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_AUTHOR;

	g_object_notify (G_OBJECT (self), "author");
}

const gchar *
gdata_query_get_updated_min (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->updated_min;
}

void
gdata_query_set_updated_min (GDataQuery *self, const gchar *updated_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->updated_min);
	self->priv->updated_min = g_strdup (updated_min);

	if (updated_min == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_UPDATED_MIN);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_UPDATED_MIN;

	g_object_notify (G_OBJECT (self), "updated-min");
}

const gchar *
gdata_query_get_updated_max (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->updated_max;
}

void
gdata_query_set_updated_max (GDataQuery *self, const gchar *updated_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->updated_max);
	self->priv->updated_max = g_strdup (updated_max);

	if (updated_max == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_UPDATED_MAX);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_UPDATED_MAX;

	g_object_notify (G_OBJECT (self), "updated-max");
}

const gchar *
gdata_query_get_published_min (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->published_min;
}

void
gdata_query_set_published_min (GDataQuery *self, const gchar *published_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->published_min);
	self->priv->published_min = g_strdup (published_min);

	if (published_min == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_PUBLISHED_MIN);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_PUBLISHED_MIN;

	g_object_notify (G_OBJECT (self), "published-min");
}

const gchar *
gdata_query_get_published_max (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->published_max;
}

void
gdata_query_set_published_max (GDataQuery *self, const gchar *published_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->published_max);
	self->priv->published_max = g_strdup (published_max);

	if (published_max == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_PUBLISHED_MAX);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_PUBLISHED_MAX;

	g_object_notify (G_OBJECT (self), "published-max");
}

gint
gdata_query_get_start_index (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), -1);
	return self->priv->start_index;
}

void
gdata_query_set_start_index (GDataQuery *self, gint start_index)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (start_index >= -1);

	self->priv->start_index = start_index;

	if (start_index == -1)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_START_INDEX);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_START_INDEX;

	g_object_notify (G_OBJECT (self), "start-index");
}

gboolean
gdata_query_get_strict (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), FALSE);
	return self->priv->strict;
}

void
gdata_query_set_strict (GDataQuery *self, gboolean strict)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	self->priv->strict = strict;

	if (strict == FALSE)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_STRICT);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_STRICT;

	g_object_notify (G_OBJECT (self), "strict");
}

gint
gdata_query_get_max_results (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), -1);
	return self->priv->max_results;
}

void
gdata_query_set_max_results (GDataQuery *self, gint max_results)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (max_results >= -1);

	self->priv->max_results = max_results;

	if (max_results == -1)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_MAX_RESULTS);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_MAX_RESULTS;

	g_object_notify (G_OBJECT (self), "max-results");
}

const gchar *
gdata_query_get_entry_id (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->entry_id;
}

void
gdata_query_set_entry_id (GDataQuery *self, const gchar *entry_id)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->entry_id);
	self->priv->entry_id = g_strdup (entry_id);

	if (entry_id == NULL)
		self->priv->parameter_mask &= (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_ENTRY_ID);
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_ENTRY_ID;

	g_object_notify (G_OBJECT (self), "entry-id");
}
