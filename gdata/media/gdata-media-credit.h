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

#ifndef GDATA_MEDIA_CREDIT_H
#define GDATA_MEDIA_CREDIT_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_MEDIA_CREDIT		(gdata_media_credit_get_type ())
#define GDATA_MEDIA_CREDIT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_MEDIA_CREDIT, GDataMediaCredit))
#define GDATA_MEDIA_CREDIT_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_MEDIA_CREDIT, GDataMediaCreditClass))
#define GDATA_IS_MEDIA_CREDIT(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_MEDIA_CREDIT))
#define GDATA_IS_MEDIA_CREDIT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_MEDIA_CREDIT))
#define GDATA_MEDIA_CREDIT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_MEDIA_CREDIT, GDataMediaCreditClass))

typedef struct _GDataMediaCreditPrivate	GDataMediaCreditPrivate;

/**
 * GDataMediaCredit:
 *
 * All the fields in the #GDataMediaCredit structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataMediaCreditPrivate *priv;
} GDataMediaCredit;

/**
 * GDataMediaCreditClass:
 *
 * All the fields in the #GDataMediaCreditClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataMediaCreditClass;

GType gdata_media_credit_get_type (void) G_GNUC_CONST;

const gchar *gdata_media_credit_get_credit (GDataMediaCredit *self);
const gchar *gdata_media_credit_get_scheme (GDataMediaCredit *self);
const gchar *gdata_media_credit_get_role (GDataMediaCredit *self);

G_END_DECLS

#endif /* !GDATA_MEDIA_CREDIT_H */
