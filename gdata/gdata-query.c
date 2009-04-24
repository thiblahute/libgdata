/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
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
 * SECTION:gdata-query
 * @short_description: GData query object
 * @stability: Unstable
 * @include: gdata/gdata-query.h
 *
 * #GDataQuery represents a collection of query parameters used in a series of queries on a #GDataService. It allows the query parameters to be
 * set, with the aim of building a query URI using gdata_query_get_query_uri(). Pagination is supported using gdata_query_next_page() and
 * gdata_query_previous_page().
 *
 * For more information on the standard GData query parameters supported by #GDataQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/gdata/docs/2.0/reference.html#Queries">online documentation</ulink>.
 **/

#include <glib.h>
#include <string.h>

#include "gdata-query.h"
#include "gdata-private.h"
#include "gdata-types.h"

typedef enum {
	GDATA_QUERY_PARAM_Q = 1 << 0,
	GDATA_QUERY_PARAM_CATEGORIES = 1 << 1,
	GDATA_QUERY_PARAM_AUTHOR = 1 << 2,
	GDATA_QUERY_PARAM_UPDATED_MIN = 1 << 3,
	GDATA_QUERY_PARAM_UPDATED_MAX = 1 << 4,
	GDATA_QUERY_PARAM_PUBLISHED_MIN = 1 << 5,
	GDATA_QUERY_PARAM_PUBLISHED_MAX = 1 << 6,
	GDATA_QUERY_PARAM_START_INDEX = 1 << 7,
	GDATA_QUERY_PARAM_IS_STRICT = 1 << 8,
	GDATA_QUERY_PARAM_MAX_RESULTS = 1 << 9,
	GDATA_QUERY_PARAM_ENTRY_ID = 1 << 10,
	GDATA_QUERY_PARAM_ALL = (1 << 11) - 1
} GDataQueryParam;

static void gdata_query_finalize (GObject *object);
static void gdata_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

struct _GDataQueryPrivate {
	guint parameter_mask; /* GDataQueryParam */

	/* Standard query parameters (see: http://code.google.com/apis/gdata/docs/2.0/reference.html#Queries) */
	gchar *q;
	gchar *categories;
	gchar *author;
	GTimeVal updated_min;
	GTimeVal updated_max;
	GTimeVal published_min;
	GTimeVal published_max;
	gint start_index;
	gboolean is_strict;
	gint max_results;
	gchar *entry_id;

	gchar *next_uri;
	gchar *previous_uri;
	gboolean use_next_uri;
	gboolean use_previous_uri;

	gchar *etag;
};

enum {
	PROP_Q = 1,
	PROP_CATEGORIES,
	PROP_AUTHOR,
	PROP_UPDATED_MIN,
	PROP_UPDATED_MAX,
	PROP_PUBLISHED_MIN,
	PROP_PUBLISHED_MAX,
	PROP_START_INDEX,
	PROP_IS_STRICT,
	PROP_MAX_RESULTS,
	PROP_ENTRY_ID,
	PROP_ETAG
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
	gobject_class->finalize = gdata_query_finalize;

	klass->get_query_uri = get_query_uri;

	/**
	 * GDataQuery:q:
	 *
	 * A full-text query string.
	 *
	 * When creating a query, list search terms separated by spaces, in the form <userinput>term1 term2 term3</userinput>.
	 * (As with all of the query parameter values, the spaces must be URL encoded.) The service returns all entries that match all of the
	 * search terms (like using AND between terms). Like Google's web search, a service searches on complete words (and related words with
	 * the same stem), not substrings.
	 *
	 * To search for an exact phrase, enclose the phrase in quotation marks: <userinput>"exact phrase"</userinput>.
	 *
	 * To exclude entries that match a given term, use the form <userinput>-term</userinput>.
	 *
	 * The search is case-insensitive.
	 *
	 * Example: to search for all entries that contain the exact phrase "Elizabeth Bennet" and the word "Darcy" but don't contain the
	 * word "Austen", use the following query: <userinput>"Elizabeth Bennet" Darcy -Austen</userinput>.
	 **/
	g_object_class_install_property (gobject_class, PROP_Q,
				g_param_spec_string ("q",
					"Query terms", "Query terms for which to search.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:categories:
	 *
	 * A category filter.
	 *
	 * You can query on multiple categories by listing multiple categories separated by slashes. The service returns all entries that match all
	 * of the categories (like using AND between terms). For example: <userinput>Fritz/Laurie</userinput> returns
	 * entries that match both categories ("Fritz" and "Laurie").
	 *
	 * To do an OR between terms, use a pipe character (<literal>|</literal>). For example: <userinput>Fritz%%7CLaurie</userinput> returns
	 * entries that match either category.
	 *
	 * An entry matches a specified category if the entry is in a category that has a matching term or label, as defined in the Atom specification.
	 * (Roughly, the "term" is the internal string used by the software to identify the category, while the "label" is the human-readable string
	 * presented to a user in a user interface.)
	 *
	 * To exclude entries that match a given category, use the form <userinput>-categoryname</userinput>.
	 *
	 * To query for a category that has a scheme – such as <literal>&lt;category scheme="urn:google.com" term="public"/&gt;</literal> – you must
	 * place the scheme in curly braces before the category name. For example: <userinput>{urn:google.com}public</userinput>. To match a category
	 * that has no scheme, use an empty pair of curly braces. If you don't specify curly braces, then categories in any scheme will match.
	 *
	 * The above features can be combined. For example: <userinput>A|-{urn:google.com}B/-C</userinput> means (A OR (NOT B)) AND (NOT C).
	 **/
	g_object_class_install_property (gobject_class, PROP_CATEGORIES,
				g_param_spec_string ("categories",
					"Category string", "Category search string.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:author:
	 *
	 * An entry author. The service returns entries where the author name and/or e-mail address match your query string.
	 **/
	g_object_class_install_property (gobject_class, PROP_AUTHOR,
				g_param_spec_string ("author",
					"Author", "Author search string.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:updated-min:
	 *
	 * Lower bound on the entry update date, inclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_UPDATED_MIN,
				g_param_spec_boxed ("updated-min",
					"Minimum update date", "Minimum date for updates on returned entries.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:updated-max:
	 *
	 * Upper bound on the entry update date, exclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_UPDATED_MAX,
				g_param_spec_boxed ("updated-max",
					"Maximum update date", "Maximum date for updates on returned entries.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:published-min:
	 *
	 * Lower bound on the entry publish date, inclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_PUBLISHED_MIN,
				g_param_spec_boxed ("published-min",
					"Minimum publish date", "Minimum date for returned entries to be published.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:published-max:
	 *
	 * Upper bound on the entry publish date, exclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_PUBLISHED_MAX,
				g_param_spec_boxed ("published-max",
					"Maximum publish date", "Maximum date for returned entries to be published.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:start-index:
	 *
	 * The one-based index of the first result to be retrieved. Use gdata_query_next_page() and gdata_query_previous_page() to
	 * implement pagination, rather than manually changing #GDataQuery:start-index.
	 **/
	g_object_class_install_property (gobject_class, PROP_START_INDEX,
				g_param_spec_int ("start-index",
					"Start index", "Zero-based result start index.",
					-1, G_MAXINT, -1,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:is-strict:
	 *
	 * Strict query parameter checking. If this is enabled, an error will be returned by the online service if a parameter is
	 * not recognised.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_STRICT,
				g_param_spec_boolean ("is-strict",
					"Strict?", "Should the server be strict about the query?",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:max-results:
	 *
	 * Maximum number of results to be retrieved. Most services have a default #GDataQuery:max-results size imposed by the server; if you wish
	 * to receive the entire feed, specify a large number such as %G_MAXINT for this property.
	 **/
	g_object_class_install_property (gobject_class, PROP_MAX_RESULTS,
				g_param_spec_int ("max-results",
					"Maximum number of results", "The maximum number of entries to return.",
					-1, G_MAXINT, -1,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:entry-id:
	 *
	 * The ID of a specific entry to be retrieved. If you specify an entry ID, you cannot specify any other parameters.
	 **/
	g_object_class_install_property (gobject_class, PROP_ENTRY_ID,
				g_param_spec_string ("entry-id",
					"Entry ID", "A specific entry ID to return.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataQuery:etag:
	 *
	 * The ETag against which to check for updates. If the server-side ETag matches this one, the requested feed hasn't changed, and is not
	 * returned unnecessarily.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_ETAG,
				g_param_spec_string ("etag",
					"ETag", "An ETag against which to check.",
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
gdata_query_finalize (GObject *object)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	g_free (priv->q);
	g_free (priv->categories);
	g_free (priv->author);
	g_free (priv->entry_id);

	g_free (priv->next_uri);
	g_free (priv->previous_uri);

	g_free (priv->etag);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_query_parent_class)->finalize (object);
}

static void
gdata_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataQueryPrivate *priv = GDATA_QUERY_GET_PRIVATE (object);

	switch (property_id) {
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
			g_value_set_boxed (value, &(priv->updated_min));
			break;
		case PROP_UPDATED_MAX:
			g_value_set_boxed (value, &(priv->updated_max));
			break;
		case PROP_PUBLISHED_MIN:
			g_value_set_boxed (value, &(priv->published_min));
			break;
		case PROP_PUBLISHED_MAX:
			g_value_set_boxed (value, &(priv->published_max));
			break;
		case PROP_START_INDEX:
			g_value_set_int (value, priv->start_index);
			break;
		case PROP_IS_STRICT:
			g_value_set_boolean (value, priv->is_strict);
			break;
		case PROP_MAX_RESULTS:
			g_value_set_int (value, priv->max_results);
			break;
		case PROP_ENTRY_ID:
			g_value_set_string (value, priv->entry_id);
			break;
		case PROP_ETAG:
			g_value_set_string (value, priv->etag);
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
	GDataQuery *self = GDATA_QUERY (object);

	switch (property_id) {
		case PROP_Q:
			gdata_query_set_q (self, g_value_get_string (value));
			break;
		case PROP_CATEGORIES:
			gdata_query_set_categories (self, g_value_get_string (value));
			break;
		case PROP_AUTHOR:
			gdata_query_set_author (self, g_value_get_string (value));
			break;
		case PROP_UPDATED_MIN:
			gdata_query_set_updated_min (self, g_value_get_boxed (value));
			break;
		case PROP_UPDATED_MAX:
			gdata_query_set_updated_max (self, g_value_get_boxed (value));
			break;
		case PROP_PUBLISHED_MIN:
			gdata_query_set_published_min (self, g_value_get_boxed (value));
			break;
		case PROP_PUBLISHED_MAX:
			gdata_query_set_published_max (self, g_value_get_boxed (value));
			break;
		case PROP_START_INDEX:
			gdata_query_set_start_index (self, g_value_get_int (value));
			break;
		case PROP_IS_STRICT:
			gdata_query_set_is_strict (self, g_value_get_boolean (value));
			break;
		case PROP_MAX_RESULTS:
			gdata_query_set_max_results (self, g_value_get_int (value));
			break;
		case PROP_ENTRY_ID:
			gdata_query_set_entry_id (self, g_value_get_string (value));
			break;
		case PROP_ETAG:
			gdata_query_set_etag (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataQueryPrivate *priv = self->priv;

	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	/* Check to see if any parameters have been set */
	if ((priv->parameter_mask & GDATA_QUERY_PARAM_ALL) == 0)
		return;

	/* If we've been provided with an entry ID, only append that */
	if (priv->entry_id != NULL) {
		g_string_append_c (query_uri, '/');
		g_string_append_uri_escaped (query_uri, priv->entry_id, NULL, TRUE);
		return;
	}

	if (priv->categories != NULL) {
		g_string_append (query_uri, "/-/");
		g_string_append_uri_escaped (query_uri, priv->categories, "/", TRUE);
	}

	/* If that's it, return */
	if ((priv->parameter_mask & (GDATA_QUERY_PARAM_ALL ^ GDATA_QUERY_PARAM_ENTRY_ID ^ GDATA_QUERY_PARAM_CATEGORIES)) == 0)
		return;

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

	if (priv->updated_min.tv_sec != 0 || priv->updated_min.tv_usec != 0) {
		gchar *updated_min;

		APPEND_SEP
		g_string_append (query_uri, "updated-min=");
		updated_min = g_time_val_to_iso8601 (&(priv->updated_min));
		g_string_append (query_uri, updated_min);
		g_free (updated_min);
	}

	if (priv->updated_max.tv_sec != 0 || priv->updated_max.tv_usec != 0) {
		gchar *updated_max;

		APPEND_SEP
		g_string_append (query_uri, "updated-max=");
		updated_max = g_time_val_to_iso8601 (&(priv->updated_max));
		g_string_append (query_uri, updated_max);
		g_free (updated_max);
	}

	if (priv->published_min.tv_sec != 0 || priv->published_min.tv_usec != 0) {
		gchar *published_min;

		APPEND_SEP
		g_string_append (query_uri, "published-min=");
		published_min = g_time_val_to_iso8601 (&(priv->published_min));
		g_string_append (query_uri, published_min);
		g_free (published_min);
	}

	if (priv->published_max.tv_sec != 0 || priv->published_max.tv_usec != 0) {
		gchar *published_max;

		APPEND_SEP
		g_string_append (query_uri, "published-max=");
		published_max = g_time_val_to_iso8601 (&(priv->published_max));
		g_string_append (query_uri, published_max);
		g_free (published_max);
	}

	if (priv->start_index > 0) {
		APPEND_SEP
		g_string_append_printf (query_uri, "start-index=%d", priv->start_index);
	}

	if (priv->is_strict == TRUE) {
		APPEND_SEP
		g_string_append (query_uri, "strict=true");
	}

	if (priv->max_results > 0) {
		APPEND_SEP
		g_string_append_printf (query_uri, "max-results=%d", priv->max_results);
	}
}

/**
 * gdata_query_new:
 * @q: a query string
 *
 * Creates a new #GDataQuery with its #GDataQuery:q property set to @q.
 *
 * Return value: a new #GDataQuery
 **/
GDataQuery *
gdata_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_QUERY, "q", q, NULL);
}

/**
 * gdata_query_new_with_limits:
 * @q: a query string
 * @start_index: a one-based start index for the results
 * @max_results: the maximum number of results to return
 *
 * Creates a new #GDataQuery with its #GDataQuery:q property set to @q, and the limits @start_index and @max_results
 * applied.
 *
 * Return value: a new #GDataQuery
 **/
GDataQuery *
gdata_query_new_with_limits (const gchar *q, gint start_index, gint max_results)
{
	return g_object_new (GDATA_TYPE_QUERY,
			     "q", q,
			     "start-index", start_index,
			     "max-results", max_results,
			     NULL);
}

/**
 * gdata_query_new_for_id:
 * @entry_id: an entry URN ID
 *
 * Creates a new #GDataQuery to query for @entry_id.
 *
 * Return value: a new #GDataQuery
 **/
GDataQuery *
gdata_query_new_for_id (const gchar *entry_id)
{
	return g_object_new (GDATA_TYPE_QUERY, "entry-id", entry_id, NULL);
}

/**
 * gdata_query_get_query_uri:
 * @self: a #GDataQuery
 * @feed_uri: the feed URI on which to build the query URI
 *
 * Builds a query URI from the given base feed URI, using the properties of the #GDataQuery. This function will take care
 * of all necessary URI escaping, so it should <emphasis>not</emphasis> be done beforehand.
 *
 * The query URI is what functions like gdata_service_query() use to query the online service.
 *
 * Return value: a query URI; free with g_free()
 **/
gchar *
gdata_query_get_query_uri (GDataQuery *self, const gchar *feed_uri)
{
	GDataQueryClass *klass;
	GString *query_uri;
	gboolean params_started;

	/* Check to see if we're paginating first */
	if (self->priv->use_next_uri == TRUE)
		return g_strdup (self->priv->next_uri);
	if (self->priv->use_previous_uri == TRUE)
		return g_strdup (self->priv->previous_uri);

	klass = GDATA_QUERY_GET_CLASS (self);
	g_assert (klass->get_query_uri != NULL);

	/* Determine whether the first param has already been appended (e.g. it exists in the feed_uri) */
	params_started = (strstr (feed_uri, "?") != NULL) ? TRUE : FALSE;

	/* Build the query URI */
	query_uri = g_string_new (feed_uri);
	klass->get_query_uri (self, feed_uri, query_uri, &params_started);

	return g_string_free (query_uri, FALSE);
}

/**
 * gdata_query_get_q:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:q property.
 *
 * Return value: the q property, or %NULL if it is unset
 **/
const gchar *
gdata_query_get_q (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->q;
}

/**
 * gdata_query_set_q:
 * @self: a #GDataQuery
 * @q: a new query string, or %NULL
 *
 * Sets the #GDataQuery:q property of the #GDataQuery to the new query string, @q.
 *
 * Set @q to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_q (GDataQuery *self, const gchar *q)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->q);
	self->priv->q = g_strdup (q);

	if (q == NULL)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_Q;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_Q;

	g_object_notify (G_OBJECT (self), "q");
}

/**
 * gdata_query_get_categories:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:categories property.
 *
 * Return value: the categories property, or %NULL if it is unset
 **/
const gchar *
gdata_query_get_categories (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->categories;
}

/**
 * gdata_query_set_categories:
 * @self: a #GDataQuery
 * @categories: the new category string, or %NULL
 *
 * Sets the #GDataQuery:categories property of the #GDataQuery to the new category string, @categories.
 *
 * Set @categories to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_categories (GDataQuery *self, const gchar *categories)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->categories);
	self->priv->categories = g_strdup (categories);

	if (categories == NULL)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_CATEGORIES;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_CATEGORIES;

	g_object_notify (G_OBJECT (self), "categories");
}

/**
 * gdata_query_get_author:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:author property.
 *
 * Return value: the author property, or %NULL if it is unset
 **/
const gchar *
gdata_query_get_author (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->author;
}

/**
 * gdata_query_set_author:
 * @self: a #GDataQuery
 * @author: the new author string, or %NULL
 *
 * Sets the #GDataQuery:author property of the #GDataQuery to the new author string, @author.
 *
 * Set @author to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_author (GDataQuery *self, const gchar *author)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->author);
	self->priv->author = g_strdup (author);

	if (author == NULL)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_AUTHOR;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_AUTHOR;

	g_object_notify (G_OBJECT (self), "author");
}

/**
 * gdata_query_get_updated_min:
 * @self: a #GDataQuery
 * @updated_min: a #GTimeVal
 *
 * Gets the #GDataQuery:updated-min property and puts it in @updated_min. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_query_get_updated_min (GDataQuery *self, GTimeVal *updated_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (updated_min != NULL);
	*updated_min = self->priv->updated_min;
}

/**
 * gdata_query_set_updated_min:
 * @self: a #GDataQuery
 * @updated_min: the new minimum update time, or %NULL
 *
 * Sets the #GDataQuery:updated-min property of the #GDataQuery to the new minimum update time, @updated_min.
 *
 * Set @updated_min to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_updated_min (GDataQuery *self, GTimeVal *updated_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	if (updated_min == NULL) {
		self->priv->updated_min.tv_sec = 0;
		self->priv->updated_min.tv_usec = 0;
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_UPDATED_MIN;
	} else {
		self->priv->updated_min = *updated_min;
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_UPDATED_MIN;
	}

	g_object_notify (G_OBJECT (self), "updated-min");
}

/**
 * gdata_query_get_updated_max:
 * @self: a #GDataQuery
 * @updated_max: a #GTimeVal
 *
 * Gets the #GDataQuery:updated-max property and puts it in @updated_max. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_query_get_updated_max (GDataQuery *self, GTimeVal *updated_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (updated_max != NULL);
	*updated_max = self->priv->updated_max;
}

/**
 * gdata_query_set_updated_max:
 * @self: a #GDataQuery
 * @updated_max: the new maximum update time, or %NULL
 *
 * Sets the #GDataQuery:updated-max property of the #GDataQuery to the new maximum update time, @updated_max.
 *
 * Set @updated_max to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_updated_max (GDataQuery *self, GTimeVal *updated_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	if (updated_max == NULL) {
		self->priv->updated_max.tv_sec = 0;
		self->priv->updated_max.tv_usec = 0;
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_UPDATED_MAX;
	} else {
		self->priv->updated_max = *updated_max;
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_UPDATED_MAX;
	}

	g_object_notify (G_OBJECT (self), "updated-max");
}

/**
 * gdata_query_get_published_min:
 * @self: a #GDataQuery
 * @published_min: a #GTimeVal
 *
 * Gets the #GDataQuery:published-min property and puts it in @published_min. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_query_get_published_min (GDataQuery *self, GTimeVal *published_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (published_min != NULL);
	*published_min = self->priv->published_min;
}

/**
 * gdata_query_set_published_min:
 * @self: a #GDataQuery
 * @published_min: the new minimum publish time, or %NULL
 *
 * Sets the #GDataQuery:published-min property of the #GDataQuery to the new minimum publish time, @published_min.
 *
 * Set @published_min to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_published_min (GDataQuery *self, GTimeVal *published_min)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	if (published_min == NULL) {
		self->priv->published_min.tv_sec = 0;
		self->priv->published_min.tv_usec = 0;
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_PUBLISHED_MIN;
	} else {
		self->priv->published_min = *published_min;
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_PUBLISHED_MIN;
	}

	g_object_notify (G_OBJECT (self), "published-min");
}

/**
 * gdata_query_get_published_max:
 * @self: a #GDataQuery
 * @published_max: a #GTimeVal
 *
 * Gets the #GDataQuery:published-max property and puts it in @published_max. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_query_get_published_max (GDataQuery *self, GTimeVal *published_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (published_max != NULL);
	*published_max = self->priv->published_max;
}

/**
 * gdata_query_set_published_max:
 * @self: a #GDataQuery
 * @published_max: the new maximum publish time, or %NULL
 *
 * Sets the #GDataQuery:published-max property of the #GDataQuery to the new maximum publish time, @published_max.
 *
 * Set @published_max to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_published_max (GDataQuery *self, GTimeVal *published_max)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	if (published_max == NULL) {
		self->priv->published_max.tv_sec = 0;
		self->priv->published_max.tv_usec = 0;
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_PUBLISHED_MAX;
	} else {
		self->priv->published_max = *published_max;
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_PUBLISHED_MAX;
	}

	g_object_notify (G_OBJECT (self), "published-max");
}

/**
 * gdata_query_get_start_index:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:start-index property.
 *
 * Return value: the start index property, or %-1 if it is unset
 **/
gint
gdata_query_get_start_index (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), -1);
	return self->priv->start_index;
}

/**
 * gdata_query_set_start_index:
 * @self: a #GDataQuery
 * @start_index: the new start index
 *
 * Sets the #GDataQuery:start-index property of the #GDataQuery to the new one-based start index, @start_index.
 *
 * Set @start_index to %-1 or %0 to unset the property in the query URI.
 **/
void
gdata_query_set_start_index (GDataQuery *self, gint start_index)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (start_index >= -1);

	/* Normalise it first */
	if (start_index <= 0)
		start_index = -1;

	self->priv->start_index = start_index;

	if (start_index == -1)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_START_INDEX;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_START_INDEX;

	g_object_notify (G_OBJECT (self), "start-index");
}

/**
 * gdata_query_is_strict:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:is-strict property.
 *
 * Return value: the strict property
 *
 * Since: 0.2.0
 **/
gboolean
gdata_query_is_strict (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), FALSE);
	return self->priv->is_strict;
}

/**
 * gdata_query_set_is_strict:
 * @self: a #GDataQuery
 * @is_strict: the new strict value
 *
 * Sets the #GDataQuery:is-strict property of the #GDataQuery to the new strict value, @is_strict.
 *
 * Since: 0.2.0
 **/
void
gdata_query_set_is_strict (GDataQuery *self, gboolean is_strict)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	self->priv->is_strict = is_strict;

	if (is_strict == FALSE)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_IS_STRICT;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_IS_STRICT;

	g_object_notify (G_OBJECT (self), "is-strict");
}

/**
 * gdata_query_get_max_results:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:max-results property.
 *
 * Return value: the maximum results property, or %-1 if it is unset
 **/
gint
gdata_query_get_max_results (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), -1);
	return self->priv->max_results;
}

/**
 * gdata_query_set_max_results:
 * @self: a #GDataQuery
 * @max_results: the new maximum results value
 *
 * Sets the #GDataQuery:max-results property of the #GDataQuery to the new maximum results value, @max_results.
 *
 * Set @max_results to %-1 to unset the property in the query URI.
 **/
void
gdata_query_set_max_results (GDataQuery *self, gint max_results)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_return_if_fail (max_results >= -1);

	self->priv->max_results = max_results;

	if (max_results == -1)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_MAX_RESULTS;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_MAX_RESULTS;

	g_object_notify (G_OBJECT (self), "max-results");
}

/**
 * gdata_query_get_entry_id:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:entry-id property.
 *
 * Return value: the entry ID property, or %NULL if it is unset
 **/
const gchar *
gdata_query_get_entry_id (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->entry_id;
}

/**
 * gdata_query_set_entry_id:
 * @self: a #GDataQuery
 * @entry_id: the new entry ID string
 *
 * Sets the #GDataQuery:entry-id property of the #GDataQuery to the new entry ID string, @entry_id.
 *
 * Set @entry_id to %NULL to unset the property in the query URI.
 **/
void
gdata_query_set_entry_id (GDataQuery *self, const gchar *entry_id)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->entry_id);
	self->priv->entry_id = g_strdup (entry_id);

	if (entry_id == NULL)
		self->priv->parameter_mask &= ~GDATA_QUERY_PARAM_ENTRY_ID;
	else
		self->priv->parameter_mask |= GDATA_QUERY_PARAM_ENTRY_ID;

	g_object_notify (G_OBJECT (self), "entry-id");
}

/**
 * gdata_query_get_etag:
 * @self: a #GDataQuery
 *
 * Gets the #GDataQuery:etag property.
 *
 * Return value: the ETag property, or %NULL if it is unset
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_query_get_etag (GDataQuery *self)
{
	g_return_val_if_fail (GDATA_IS_QUERY (self), NULL);
	return self->priv->etag;
}

/**
 * gdata_query_set_etag:
 * @self: a #GDataQuery
 * @etag: the new ETag
 *
 * Sets the #GDataQuery:etag property of the #GDataQuery to the new ETag, @etag.
 *
 * Set @etag to %NULL to not check against the server-side ETag.
 *
 * Since: 0.2.0
 **/
void
gdata_query_set_etag (GDataQuery *self, const gchar *etag)
{
	g_return_if_fail (GDATA_IS_QUERY (self));

	g_free (self->priv->etag);
	self->priv->etag = g_strdup (etag);
	g_object_notify (G_OBJECT (self), "etag");
}

void
_gdata_query_set_next_uri (GDataQuery *self, const gchar *next_uri)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_free (self->priv->next_uri);
	self->priv->next_uri = g_strdup (next_uri);
	self->priv->use_next_uri = FALSE;
	self->priv->use_previous_uri = FALSE;
}

void
_gdata_query_set_previous_uri (GDataQuery *self, const gchar *previous_uri)
{
	g_return_if_fail (GDATA_IS_QUERY (self));
	g_free (self->priv->previous_uri);
	self->priv->previous_uri = g_strdup (previous_uri);
	self->priv->use_next_uri = FALSE;
	self->priv->use_previous_uri = FALSE;
}

/**
 * gdata_query_next_page:
 * @self: a #GDataQuery
 *
 * Changes the state of the #GDataQuery such that when gdata_query_get_query_uri() is next called, it will build the
 * query URI for the next page in the result set.
 *
 * Ideally, the URI of the next page is retrieved from a feed automatically when gdata_service_query() is called, but
 * gdata_query_next_page() will fall back to using #GDataQuery:start-index to emulate true pagination if this fails.
 *
 * You <emphasis>should not</emphasis> implement pagination manually using #GDataQuery:start-index.
 **/
void
gdata_query_next_page (GDataQuery *self)
{
	GDataQueryPrivate *priv = self->priv;

	if (priv->next_uri != NULL) {
		priv->use_next_uri = TRUE;
		priv->use_previous_uri = FALSE;
	} else {
		priv->start_index += priv->max_results;
	}

	/* Our current ETag will not be relevant */
	g_free (priv->etag);
	priv->etag = NULL;
}

/**
 * gdata_query_previous_page:
 * @self: a #GDataQuery
 *
 * Changes the state of the #GDataQuery such that when gdata_query_get_query_uri() is next called, it will build the
 * query URI for the previous page in the result set.
 *
 * See the documentation for gdata_query_next_page() for an explanation of how query URIs from the feeds are used to this end.
 *
 * Return value: %TRUE if there is a previous page and it has been switched to, %FALSE otherwise
 **/
gboolean
gdata_query_previous_page (GDataQuery *self)
{
	GDataQueryPrivate *priv = self->priv;

	if (priv->next_uri != NULL) {
		priv->use_previous_uri = TRUE;
		priv->use_next_uri = FALSE;
	} else if (priv->start_index < priv->max_results) {
		return FALSE;
	} else {
		priv->start_index -= priv->max_results;
	}

	/* Our current ETag will not be relevant */
	g_free (priv->etag);
	priv->etag = NULL;

	return TRUE;
}
