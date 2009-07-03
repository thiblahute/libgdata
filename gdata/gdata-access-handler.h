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

#ifndef GDATA_ACCESS_HANDLER_H
#define GDATA_ACCESS_HANDLER_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-feed.h>
#include <gdata/gdata-service.h>
#include <gdata/gdata-access-rule.h>

G_BEGIN_DECLS

#define GDATA_TYPE_ACCESS_HANDLER		(gdata_access_handler_get_type ())
#define GDATA_ACCESS_HANDLER(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_ACCESS_HANDLER, GDataAccessHandler))
#define GDATA_ACCESS_HANDLER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_ACCESS_HANDLER, GDataAccessHandlerIface))
#define GDATA_IS_ACCESS_HANDLER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_ACCESS_HANDLER))
#define GDATA_ACCESS_HANDLER_GET_IFACE(o)	(G_TYPE_INSTANCE_GET_INTERFACE ((o), GDATA_TYPE_ACCESS_HANDLER, GDataAccessHandlerIface))

/**
 * GDataAccessHandler:
 *
 * All the fields in the #GDataAccessHandler structure are private and should never be accessed directly.
 *
 * Since: 0.3.0
 **/
typedef struct _GDataAccessHandler		GDataAccessHandler; /* dummy typedef */

/**
 * GDataAccessHandlerIface:
 * @parent: the parent type
 * @is_owner_rule: a function to return whether the given #GDataAccessRule has the role of an owner (of a #GDataAccessHandler).
 *
 * The class structure for the #GDataAccessHandler interface.
 *
 * Since: 0.3.0
 **/
typedef struct {
	GTypeInterface parent;

	gboolean (*is_owner_rule) (GDataAccessRule *rule);
} GDataAccessHandlerIface;

GType gdata_access_handler_get_type (void) G_GNUC_CONST;

GDataFeed *gdata_access_handler_get_rules (GDataAccessHandler *self, GDataService *service, GCancellable *cancellable,
					   GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
					   GError **error) G_GNUC_WARN_UNUSED_RESULT;
GDataAccessRule *gdata_access_handler_insert_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule,
						   GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;
GDataAccessRule *gdata_access_handler_update_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule,
						   GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;
gboolean gdata_access_handler_delete_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule,
					   GCancellable *cancellable, GError **error);

G_END_DECLS

#endif /* !GDATA_ACCESS_HANDLER_H */
