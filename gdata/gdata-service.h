/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008 <philip@tecnocode.co.uk>
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

#ifndef GDATA_SERVICE_H
#define GDATA_SERVICE_H

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>
#include <libxml/parser.h>

#include "gdata-feed.h"

G_BEGIN_DECLS

typedef enum {
	GDATA_SERVICE_ERROR_AUTHENTICATING = 1,
	GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
	GDATA_SERVICE_ERROR_WITH_QUERY,
	GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT
} GDataServiceError;

typedef GDataEntry *(*GDataEntryParserFunc) (xmlDoc *doc, xmlNode *node, GError **error);

#define GDATA_TYPE_SERVICE		(gdata_service_get_type ())
#define GDATA_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_SERVICE, GDataService))
#define GDATA_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_SERVICE, GDataServiceClass))
#define GDATA_IS_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_SERVICE))
#define GDATA_IS_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_SERVICE))
#define GDATA_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_SERVICE, GDataServiceClass))

#define GDATA_SERVICE_ERROR		gdata_service_error_quark ()

typedef struct _GDataServicePrivate	GDataServicePrivate;

typedef struct {
	GObject parent;
	GDataServicePrivate *priv;
} GDataService;

typedef struct {
	GObjectClass parent;

	const gchar *service_name;
	const gchar *authentication_uri;

	gboolean (*parse_authentication_response) (GDataService *self, const gchar *response_body, GError **error);
	void (*append_query_headers) (GDataService *self, SoupMessage *message);
} GDataServiceClass;

GType gdata_service_get_type (void);
GQuark gdata_service_error_quark (void);

gboolean gdata_service_authenticate (GDataService *self, const gchar *username, const gchar *password, GError **error);
gboolean gdata_service_authenticate_async (GDataService *self, const gchar *username, const gchar *password, GError **error);
gboolean gdata_service_is_logged_in (GDataService *self);

#include "gdata-query.h"

gboolean gdata_service_query_async (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func, GError **error);
GDataFeed *gdata_service_query (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func, GError **error);

const gchar *gdata_service_get_client_id (GDataService *self);

G_END_DECLS

#endif /* !GDATA_SERVICE_H */
