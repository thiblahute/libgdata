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
 * SECTION:gdata-feed
 * @short_description: GData feed object
 * @stability: Unstable
 * @include: gdata/gdata-feed.h
 *
 * #GDataFeed is a list of entries (#GDataEntry) returned as the result of a query to a #GDataService, or given as the input to another
 * operation on the online service. It also has pieces of data associated with the query on the #GDataService, such as the query title
 * or timestamp when it was last updated.
 *
 * Each #GDataEntry represents a single object on the online service, such as a playlist, video or calendar entry, and the #GDataFeed
 * represents a collection of similar objects.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-feed.h"
#include "gdata-entry.h"
#include "gdata-types.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-parser.h"

static void gdata_feed_finalize (GObject *object);
static void gdata_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataFeedPrivate {
	GList *entries;
	gchar *title;
	gchar *subtitle;
	gchar *id;
	gchar *etag;
	GTimeVal updated;
	GList *categories;
	gchar *logo;
	GList *links;
	GList *authors;
	GDataGenerator *generator;
	guint items_per_page;
	guint start_index;
	guint total_results;
	gchar *extra_xml;
};

enum {
	PROP_ID = 1,
	PROP_ETAG,
	PROP_UPDATED,
	PROP_TITLE,
	PROP_SUBTITLE,
	PROP_LOGO,
	PROP_GENERATOR,
	PROP_ITEMS_PER_PAGE,
	PROP_START_INDEX,
	PROP_TOTAL_RESULTS
};

G_DEFINE_TYPE (GDataFeed, gdata_feed, G_TYPE_OBJECT)
#define GDATA_FEED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_FEED, GDataFeedPrivate))

static void
gdata_feed_class_init (GDataFeedClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataFeedPrivate));

	gobject_class->set_property = gdata_feed_set_property;
	gobject_class->get_property = gdata_feed_get_property;
	gobject_class->finalize = gdata_feed_finalize;

	/**
	 * GDataFeed:title:
	 *
	 * The title of the feed.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_title">atom:title</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title of the feed.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:subtitle:
	 *
	 * The subtitle of the feed.
	 *
	 * API reference: <ulink type="http" url="http://atomenabled.org/developers/syndication/">atom:subtitle</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_SUBTITLE,
				g_param_spec_string ("subtitle",
					"Subtitle", "The subtitle of the feed.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:id:
	 *
	 * The unique and permanent URN ID for the feed.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_id">atom:id</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_ID,
				g_param_spec_string ("id",
					"ID", "The unique and permanent URN ID for the feed.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:etag:
	 *
	 * The unique ETag for this version of the feed. See the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/2.0/reference.html#ResourceVersioning">online documentation</ulink> for
	 * more information.
	 **/
	g_object_class_install_property (gobject_class, PROP_ETAG,
				g_param_spec_string ("etag",
					"ETag", "The unique ETag for this version of the feed.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));


	/**
	 * GDataFeed:updated:
	 *
	 * The time the feed was last updated.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_updated">
	 * atom:updated</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The time the feed was last updated.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:logo:
	 *
	 * The URI of a logo for the feed.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_logo">atom:logo</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_LOGO,
				g_param_spec_string ("logo",
					"Logo", "The URI of a logo for the feed.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:generator:
	 *
	 * Details of the software used to generate the feed.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_generator">
	 * atom:generator</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_GENERATOR,
				g_param_spec_pointer ("generator",
					"Generator", "Details of the software used to generate the feed.",
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:items-per-page:
	 *
	 * The number of items per results page feed.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_openSearch:itemsPerPage">
	 * openSearch:itemsPerPage</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_ITEMS_PER_PAGE,
				g_param_spec_uint ("items-per-page",
					"Items per page", "The number of items per results page feed.",
					0, G_MAXINT, 0,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:start-index:
	 *
	 * The one-based index of the first item in the results feed.
	 *
	 * This should <emphasis>not</emphasis> be used manually for pagination. Instead, use a #GDataQuery and call its gdata_query_next_page()
	 * or gdata_query_previous_page() functions before making the query to the service.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_openSearch:startIndex">
	 * openSearch:startIndex</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_START_INDEX,
				g_param_spec_uint ("start-index",
					"Start index", "The one-based index of the first item in the results feed.",
					1, G_MAXINT, 1,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataFeed:total-results:
	 *
	 * The number of items in the result set for the feed, including those on other pages.
	 *
	 * This should <emphasis>not</emphasis> be used manually for pagination. Instead, use a #GDataQuery and call its gdata_query_next_page()
	 * or gdata_query_previous_page() functions before making the query to the service.
	 *
	 * API reference: <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_openSearch:totalResults">
	 * openSearch:totalResults</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_TOTAL_RESULTS,
				g_param_spec_uint ("total-results",
					"Total results", "The total number of results in the feed.",
					0, 1000000, 0,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_feed_init (GDataFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_FEED, GDataFeedPrivate);
}

static void
gdata_feed_finalize (GObject *object)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

	g_list_foreach (priv->entries, (GFunc) g_object_unref, NULL); /* TODO: move to dispose */
	g_list_free (priv->entries);
	g_free (priv->title);
	g_free (priv->subtitle);
	g_free (priv->id);
	g_free (priv->etag);
	g_list_foreach (priv->categories, (GFunc) gdata_category_free, NULL);
	g_list_free (priv->categories);
	g_free (priv->logo);
	g_list_foreach (priv->links, (GFunc) gdata_link_free, NULL);
	g_list_free (priv->links);
	g_list_foreach (priv->authors, (GFunc) gdata_author_free, NULL);
	g_list_free (priv->authors);
	gdata_generator_free (priv->generator);

	g_free (priv->extra_xml);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_feed_parent_class)->finalize (object);
}

static void
gdata_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_SUBTITLE:
			g_value_set_string (value, priv->subtitle);
			break;
		case PROP_ID:
			g_value_set_string (value, priv->id);
			break;
		case PROP_ETAG:
			g_value_set_string (value, priv->etag);
			break;
		case PROP_UPDATED:
			g_value_set_boxed (value, &(priv->updated));
			break;
		case PROP_LOGO:
			g_value_set_string (value, priv->logo);
			break;
		case PROP_GENERATOR:
			g_value_set_pointer (value, priv->generator);
			break;
		case PROP_ITEMS_PER_PAGE:
			g_value_set_uint (value, priv->items_per_page);
			break;
		case PROP_START_INDEX:
			g_value_set_uint (value, priv->start_index);
			break;
		case PROP_TOTAL_RESULTS:
			g_value_set_uint (value, priv->total_results);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);
	GTimeVal *timeval;

	switch (property_id) {
		case PROP_TITLE:
			priv->title = g_value_dup_string (value);
			break;
		case PROP_SUBTITLE:
			priv->subtitle = g_value_dup_string (value);
			break;
		case PROP_ID:
			priv->id = g_value_dup_string (value);
			break;
		case PROP_ETAG:
			priv->etag = g_value_dup_string (value);
			break;
		case PROP_UPDATED:
			timeval = g_value_get_boxed (value);
			priv->updated = *timeval;
			break;
		case PROP_LOGO:
			priv->logo = g_value_dup_string (value);
			break;
		case PROP_GENERATOR:
			priv->generator = g_value_get_pointer (value);
			break;
		case PROP_ITEMS_PER_PAGE:
			priv->items_per_page = g_value_get_uint (value);
			break;
		case PROP_START_INDEX:
			priv->start_index = g_value_get_uint (value);
			break;
		case PROP_TOTAL_RESULTS:
			priv->total_results = g_value_get_uint (value);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

typedef struct {
	GDataQueryProgressCallback progress_callback;
	gpointer progress_user_data;
	GDataEntry *entry;
	guint entry_i;
	guint total_results;
} ProgressCallbackData;

static gboolean
progress_callback_idle (ProgressCallbackData *data)
{
	data->progress_callback (data->entry, data->entry_i, data->total_results, data->progress_user_data);
	g_object_unref (data->entry);
	g_slice_free (ProgressCallbackData, data);
	return FALSE;
}

GDataFeed *
_gdata_feed_new_from_xml (const gchar *xml, gint length, GType entry_type,
			  GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
	GDataFeed *feed = NULL;
	xmlDoc *doc;
	xmlNode *node;
	xmlChar *title = NULL, *subtitle = NULL, *id = NULL, *logo = NULL, *etag = NULL;
	GTimeVal updated = { 0, };
	GDataGenerator *generator = NULL;
	guint entry_i = 0, total_results = 0, start_index = 0, items_per_page = 0;
	GList *entries = NULL, *categories = NULL, *links = NULL, *authors = NULL;
	GString *extra_xml;

	g_return_val_if_fail (xml != NULL, NULL);

	if (length == -1)
		length = strlen (xml);

	/* Parse the XML */
	doc = xmlReadMemory (xml, length, "feed.xml", NULL, 0);
	if (doc == NULL) {
		xmlError *xml_error = xmlGetLastError ();
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_PARSING_STRING,
			     _("Error parsing XML: %s"),
			     xml_error->message);
		return NULL;
	}

	/* Get the root element */
	node = xmlDocGetRootElement (doc);
	if (node == NULL) {
		/* XML document's empty */
		xmlFreeDoc (doc);
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_EMPTY_DOCUMENT,
			     _("Error parsing XML: %s"),
			     _("Empty document."));
		return NULL;
	}

	if (xmlStrcmp (node->name, (xmlChar*) "feed") != 0) {
		/* No <feed> element (required) */
		xmlFreeDoc (doc);
		gdata_parser_error_required_element_missing ("feed", "root", error);
		return NULL;
	}

	/* Get the ETag first */
	etag = xmlGetProp (node, (xmlChar*) "etag");

	extra_xml = g_string_new ("");
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (xmlStrcmp (node->name, (xmlChar*) "entry") == 0) {
			/* atom:entry */
			GDataEntry *entry = _gdata_entry_new_from_xml_node (entry_type, doc, node, error);
			if (entry == NULL)
				goto error;

			/* Call the progress callback in the main thread */
			if (progress_callback != NULL) {
				ProgressCallbackData *data;

				/* Build the data for the callback */
				data = g_slice_new (ProgressCallbackData);
				data->progress_callback = progress_callback;
				data->progress_user_data = progress_user_data;
				data->entry = g_object_ref (entry);
				data->entry_i = entry_i;
				data->total_results = MIN (items_per_page, total_results);

				/* Send the callback; use G_PRIORITY_DEFAULT rather than G_PRIORITY_DEFAULT_IDLE
				 * to contend with the priorities used by the callback functions in GAsyncResult */
				g_idle_add_full (G_PRIORITY_DEFAULT, (GSourceFunc) progress_callback_idle, data, NULL);
			}

			entry_i++;
			entries = g_list_prepend (entries, entry);
		} else if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
			/* atom:title */
			if (title != NULL) {
				gdata_parser_error_duplicate_element ("title", "feed", error);
				goto error;
			}

			title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		} else if (xmlStrcmp (node->name, (xmlChar*) "subtitle") == 0) {
			/* atom:subtitle */
			if (subtitle != NULL) {
				gdata_parser_error_duplicate_element ("subtitle", "feed", error);
				goto error;
			}

			subtitle = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) {
			/* atom:id */
			if (id != NULL) {
				gdata_parser_error_duplicate_element ("id", "feed", error);
				goto error;
			}

			id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) {
			/* atom:updated */
			xmlChar *updated_string;

			/* Duplicate checking */
			if (updated.tv_sec != 0 || updated.tv_usec != 0) {
				gdata_parser_error_duplicate_element ("updated", "feed", error);
				goto error;
			}

			/* Parse the string */
			updated_string = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
			if (g_time_val_from_iso8601 ((gchar*) updated_string, &updated) == FALSE) {
				gdata_parser_error_not_iso8601_format ("updated", "feed", (gchar*) updated_string, error);
				xmlFree (updated_string);
				goto error;
			}

			xmlFree (updated_string);
		} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
			/* atom:category */
			xmlChar *scheme, *term, *label;
			GDataCategory *category;

			scheme = xmlGetProp (node, (xmlChar*) "scheme");
			term = xmlGetProp (node, (xmlChar*) "term");
			label = xmlGetProp (node, (xmlChar*) "label");

			category = gdata_category_new ((gchar*) term, (gchar*) scheme, (gchar*) label);
			categories = g_list_prepend (categories, category);

			xmlFree (scheme);
			xmlFree (term);
			xmlFree (label);
		} else if (xmlStrcmp (node->name, (xmlChar*) "logo") == 0) {
			/* atom:logo */
			if (logo != NULL) {
				gdata_parser_error_duplicate_element ("logo", "feed", error);
				goto error;
			}

			logo = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		} else if (xmlStrcmp (node->name, (xmlChar*) "link") == 0) {
			/* atom:link */
			xmlChar *href, *rel, *type, *hreflang, *link_title, *link_length;
			gint length_int;
			GDataLink *link;

			href = xmlGetProp (node, (xmlChar*) "href");
			rel = xmlGetProp (node, (xmlChar*) "rel");
			type = xmlGetProp (node, (xmlChar*) "type");
			hreflang = xmlGetProp (node, (xmlChar*) "hreflang");
			link_title = xmlGetProp (node, (xmlChar*) "title");
			link_length = xmlGetProp (node, (xmlChar*) "length");

			if (link_length == NULL)
				length_int = -1;
			else
				length_int = strtoul ((gchar*) link_length, NULL, 10);

			link = gdata_link_new ((gchar*) href, (gchar*) rel, (gchar*) type, (gchar*) hreflang, (gchar*) link_title, length_int);
			links = g_list_prepend (links, link);

			xmlFree (href);
			xmlFree (rel);
			xmlFree (type);
			xmlFree (hreflang);
			xmlFree (link_title);
			xmlFree (link_length);
		} else if (xmlStrcmp (node->name, (xmlChar*) "author") == 0) {
			/* atom:author */
			GDataAuthor *author;
			xmlNode *author_node;
			xmlChar *name = NULL, *uri = NULL, *email = NULL;

			author_node = node->xmlChildrenNode;
			while (author_node != NULL) {
				if (xmlStrcmp (author_node->name, (xmlChar*) "name") == 0) {
					name = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
				} else if (xmlStrcmp (author_node->name, (xmlChar*) "uri") == 0) {
					uri = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
				} else if (xmlStrcmp (author_node->name, (xmlChar*) "email") == 0) {
					email = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
				} else {
					gdata_parser_error_unhandled_element ((gchar*) author_node->ns->prefix, (gchar*) author_node->name, "author", error);
					xmlFree (name);
					xmlFree (uri);
					xmlFree (email);
					goto error;
				}

				author_node = author_node->next;
			}

			author = gdata_author_new ((gchar*) name, (gchar*) uri, (gchar*) email);
			authors = g_list_prepend (authors, author);

			xmlFree (name);
			xmlFree (uri);
			xmlFree (email);
		} else if (xmlStrcmp (node->name, (xmlChar*) "generator") == 0) {
			/* atom:generator */
			xmlChar *name, *uri, *version;

			/* Duplicate checking */
			if (generator != NULL) {
				gdata_parser_error_duplicate_element ("generator", "feed", error);
				goto error;
			}

			/* Parse the element's parameters */
			name = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
			uri = xmlGetProp (node, (xmlChar*) "uri");
			version = xmlGetProp (node, (xmlChar*) "version");

			generator = gdata_generator_new ((gchar*) name, (gchar*) uri, (gchar*) version);

			xmlFree (name);
			xmlFree (uri);
			xmlFree (version);
		} else if (xmlStrcmp (node->name, (xmlChar*) "totalResults") == 0) {
			/* openSearch:totalResults */
			xmlChar *total_results_string;

			/* Duplicate checking */
			if (total_results != 0) {
				gdata_parser_error_duplicate_element ("totalResults", "feed", error);
				goto error;
			}

			/* Parse the number */
			total_results_string = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
			if (total_results_string == NULL) {
				gdata_parser_error_required_content_missing ("openSearch:totalResults", error);
				goto error;
			}

			total_results = strtoul ((gchar*) total_results_string, NULL, 10);
			xmlFree (total_results_string);
		} else if (xmlStrcmp (node->name, (xmlChar*) "startIndex") == 0) {
			/* openSearch:startIndex */
			xmlChar *start_index_string;

			/* Duplicate checking */
			if (start_index != 0) {
				gdata_parser_error_duplicate_element ("startIndex", "feed", error);
				goto error;
			}

			/* Parse the number */

			start_index_string = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
			if (start_index_string == NULL) {
				gdata_parser_error_required_content_missing ("openSearch:startIndex", error);
				goto error;
			}

			start_index = strtoul ((gchar*) start_index_string, NULL, 10);
			xmlFree (start_index_string);
		} else if (xmlStrcmp (node->name, (xmlChar*) "itemsPerPage") == 0) {
			/* openSearch:itemsPerPage */
			xmlChar *items_per_page_string;

			/* Duplicate checking */
			if (items_per_page != 0) {
				gdata_parser_error_duplicate_element ("itemsPerPage", "feed", error);
				goto error;
			}

			/* Parse the number */
			items_per_page_string = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
			if (items_per_page_string == NULL) {
				gdata_parser_error_required_content_missing ("openSearch:itemsPerPage", error);
				goto error;
			}

			items_per_page = strtoul ((gchar*) items_per_page_string, NULL, 10);
			xmlFree (items_per_page_string);
		} else {
			/* Unhandled XML */
			xmlBuffer *buffer = xmlBufferCreate ();
			xmlNodeDump (buffer, doc, node, 0, 0);
			g_string_append (extra_xml, (gchar*) xmlBufferContent (buffer));
			g_message ("Unhandled XML: %s", (gchar*) xmlBufferContent (buffer));
			xmlBufferFree (buffer);
		}

		node = node->next;
	}

	/* Check for missing required elements */
	if (title == NULL) {
		gdata_parser_error_required_element_missing ("title", "feed", error);
		goto error;
	}
	if (id == NULL) {
		gdata_parser_error_required_element_missing ("id", "feed", error);
		goto error;
	}
	if (updated.tv_sec == 0 && updated.tv_usec == 0) {
		gdata_parser_error_required_element_missing ("updated", "feed", error);
		goto error;
	}

	/* Reverse our lists of stuff */
	entries = g_list_reverse (entries);
	categories = g_list_reverse (categories);
	links = g_list_reverse (links);
	authors = g_list_reverse (authors);

	/* Create the feed */
	feed = g_object_new (GDATA_TYPE_FEED,
			     "title", (gchar*) title,
			     "subtitle", (gchar*) subtitle,
			     "id", (gchar*) id,
			     "etag", (gchar*) etag,
			     "updated", &updated,
			     "logo", (gchar*) logo,
			     "generator", generator,
			     "total-results", total_results,
			     "start-index", start_index,
			     "items-per-page", items_per_page,
			     NULL);

	feed->priv->entries = entries;
	feed->priv->categories = categories;
	feed->priv->links = links;
	feed->priv->authors = authors;

	/* Set the lists and generator to NULL so they aren't freed --- the memory belongs to the GDataFeed now */
	entries = categories = links = authors = NULL;
	generator = NULL;

error:
	xmlFree (title);
	xmlFree (subtitle);
	xmlFree (id);
	xmlFree (etag);
	xmlFree (logo);
	gdata_generator_free (generator);
	g_list_foreach (entries, (GFunc) g_object_unref, NULL);
	g_list_free (entries);
	g_list_foreach (categories, (GFunc) gdata_category_free, NULL);
	g_list_free (categories);
	g_list_foreach (links, (GFunc) gdata_link_free, NULL);
	g_list_free (links);
	g_list_foreach (authors, (GFunc) gdata_author_free, NULL);
	g_list_free (authors);

	/* Store any unhandled XML for future use */
	if (feed != NULL)
		feed->priv->extra_xml = g_string_free (extra_xml, FALSE);
	else
		g_string_free (extra_xml, TRUE);

	xmlFreeDoc (doc);

	return feed;
}

/**
 * gdata_feed_get_entries:
 * @self: a #GDataFeed
 *
 * Returns a list of the entries contained in this feed.
 *
 * Return value: a #GList of #GDataEntry<!-- -->s; free the list with g_list_free(), but do not unref the entries
 **/
GList *
gdata_feed_get_entries (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->entries;
}

static gint
entry_compare_cb (const GDataEntry *entry, const gchar *id)
{
	return strcmp (gdata_entry_get_id (GDATA_ENTRY (entry)), id);
}

/**
 * gdata_feed_look_up_entry:
 * @self: a #GDataFeed
 * @id: the entry's ID
 *
 * Returns the entry in the feed with the given @id, if found.
 *
 * Return value: the #GDataEntry, or %NULL
 **/
GDataEntry *
gdata_feed_look_up_entry (GDataFeed *self, const gchar *id)
{
	GList *element;

	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	g_return_val_if_fail (id != NULL, NULL);

	element = g_list_find_custom (self->priv->entries, id, (GCompareFunc) entry_compare_cb);
	if (element == NULL)
		return NULL;
	return GDATA_ENTRY (element->data);
}

/**
 * gdata_feed_get_categories:
 * @self: a #GDataFeed
 *
 * Returns a list of the categories listed in this feed.
 *
 * Return value: a #GList of #GDataCategory<!-- -->s; free the list with g_list_free(), but do not free the categories
 **/
GList *
gdata_feed_get_categories (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->categories;
}

/**
 * gdata_feed_get_links:
 * @self: a #GDataFeed
 *
 * Returns a list of the links listed in this feed.
 *
 * Return value: a #GList of #GDataLink<!-- -->s; free the list with g_list_free(), but do not free the links
 **/
GList *
gdata_feed_get_links (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->links;
}

static gint
link_compare_cb (const GDataLink *link, const gchar *rel)
{
	return strcmp (link->rel, rel);
}

/**
 * gdata_feed_look_up_link:
 * @self: a #GDataFeed
 * @rel: the value of the <structfield>rel</structfield> attribute of the desired link
 *
 * Looks up a link by <structfield>rel</structfield> value from the list of links in the feed.
 *
 * Return value: a #GDataLink, or %NULL if one was not found
 **/
GDataLink *
gdata_feed_look_up_link (GDataFeed *self, const gchar *rel)
{
	GList *element;

	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	g_return_val_if_fail (rel != NULL, NULL);

	element = g_list_find_custom (self->priv->links, rel, (GCompareFunc) link_compare_cb);
	if (element == NULL)
		return NULL;
	return (GDataLink*) (element->data);
}

/**
 * gdata_feed_get_authors:
 * @self: a #GDataFeed
 *
 * Returns a list of the authors listed in this feed.
 *
 * Return value: a #GList of #GDataAuthor<!-- -->s; free the list with g_list_free(), but do not free the authors
 **/
GList *
gdata_feed_get_authors (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->authors;
}

/**
 * gdata_feed_get_title:
 * @self: a #GDataFeed
 *
 * Returns the title of the feed.
 *
 * Return value: the feed's title
 **/
const gchar *
gdata_feed_get_title (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->title;
}

/**
 * gdata_feed_get_subtitle:
 * @self: a #GDataFeed
 *
 * Returns the subtitle of the feed.
 *
 * Return value: the feed's subtitle, or %NULL
 **/
const gchar *
gdata_feed_get_subtitle (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->subtitle;
}

/**
 * gdata_feed_get_id:
 * @self: a #GDataFeed
 *
 * Returns the feed's unique and permanent URN ID.
 *
 * Return value: the feed's ID
 **/
const gchar *
gdata_feed_get_id (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->id;
}

/**
 * gdata_feed_get_etag:
 * @self: a #GDataFeed
 *
 * Returns the feed's unique ETag for this version.
 *
 * Return value: the feed's ETag
 **/
const gchar *
gdata_feed_get_etag (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->etag;
}

/**
 * gdata_feed_get_updated:
 * @self: a #GDataFeed
 * @updated: a #GTimeVal
 *
 * Puts the time the feed was last updated into @updated.
 **/
void
gdata_feed_get_updated (GDataFeed *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (updated != NULL);
	*updated = self->priv->updated;
}

/**
 * gdata_feed_get_logo:
 * @self: a #GDataFeed
 *
 * Returns the logo URI of the feed.
 *
 * Return value: the feed's logo URI, or %NULL
 **/
const gchar *
gdata_feed_get_logo (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->logo;
}

/**
 * gdata_feed_get_generator:
 * @self: a #GDataFeed
 *
 * Returns details about the software which generated the feed.
 *
 * Return value: a #GDataGenerator, or %NULL
 **/
GDataGenerator *
gdata_feed_get_generator (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->generator;
}

/**
 * gdata_feed_get_items_per_page:
 * @self: a #GDataFeed
 *
 * Returns the number of items per results page feed.
 *
 * Return value: the number of items per results page feed, or 0 on error
 **/
guint
gdata_feed_get_items_per_page (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->items_per_page;
}

/**
 * gdata_feed_get_start_index:
 * @self: a #GDataFeed
 *
 * Returns the one-based start index of the results feed in the result set.
 *
 * Return value: the one-based start index, or 0 on error
 **/
guint
gdata_feed_get_start_index (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->start_index;
}

/**
 * gdata_feed_get_total_results:
 * @self: a #GDataFeed
 *
 * Returns the total number of results in the result set, including results on other
 * pages.
 *
 * Return value: the total number of results, or 0 on error
 **/
guint
gdata_feed_get_total_results (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->total_results;
}
