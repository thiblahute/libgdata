/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Thibault Saunier <saunierthibault@gmail.com
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

#ifndef GDATA_DOCUMENTS_FEED_H
#define GDATA_DOCUMENTS_FEED_H

#include <glib.h>
#include <glib-object.h>

#include "gdata-documents-entry.h"
#include "gdata-documents-spreadsheet.h"
#include "gdata-documents-text.h"
#include "gdata-documents-presentation.h"
#include "gdata-documents-folder.h"
#include <gdata/gdata-atom.h>
#include <gdata/gdata-feed.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_FEED			(gdata_documents_feed_get_type ())
#define GDATA_DOCUMENTS_FEED(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_DOCUMENTS_FEED, GDataDocumentsFeed))
#define GDATA_DOCUMENTS_FEED_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_DOCUMENTS_FEED, GDataDocumentsFeedClass))
#define GDATA_IS_DOCUMENTS_FEED(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_DOCUMENTS_FEED))
#define GDATA_IS_DOCUMENTS_FEED_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_DOCUMENTS_FEED))
#define GDATA_DOCUMENTS_FEED_GET_CLASS(o)		(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_DOCUMENTS_FEED, GDataDocumentsFeedClass))

typedef struct _GDataDocumentsFeedPrivate	GDataDocumentsFeedPrivate;

/**
 * GDataDocumentsFeed:
 *
 * All the fields in the #GDataDocumentsFeed structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataFeed parent;
	GDataDocumentsFeedPrivate *priv;
} GDataDocumentsFeed;

/**
 * GDataDocumentsFeedClass:
 *
 * All the fields in the #GDataDocumentsFeedClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataFeedClass parent;
} GDataDocumentsFeedClass;

GType gdata_documents_feed_get_type (void) G_GNUC_CONST;

/*GList *gdata_feed_get_entries (GDataDocumentsFeed *self);
GDataEntry *gdata_feed_look_up_entry (GDataDocumentsFeed *self, const gchar *id);
GList *gdata_feed_get_categories (GDataDocumentsFeed *self);
GList *gdata_feed_get_links (GDataDocumentsFeed *self);
GDataLink *gdata_feed_look_up_link (GDataDocumentsFeed *self, const gchar *rel);
GList *gdata_feed_get_authors (GDataDocumentsFeed *self);

const gchar *gdata_feed_get_title (GDataDocumentsFeed *self);
const gchar *gdata_feed_get_subtitle (GDataDocumentsFeed *self);
const gchar *gdata_feed_get_id (GDataDocumentsFeed *self);
const gchar *gdata_feed_get_etag (GDataDocumentsFeed *self);
void gdata_feed_get_updated (GDataDocumentsFeed *self, GTimeVal *updated);
const gchar *gdata_feed_get_logo (GDataDocumentsFeed *self);
GDataGenerator *gdata_feed_get_generator (GDataDocumentsFeed *self);
guint gdata_feed_get_items_per_page (GDataDocumentsFeed *self);
guint gdata_feed_get_start_index (GDataDocumentsFeed *self);
guint gdata_feed_get_total_results (GDataDocumentsFeed *self);*/

G_END_DECLS

#endif /* !GDATA_DOCUMENTS_FEED_H */
