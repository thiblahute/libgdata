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

#ifndef GDATA_GD_REMINDER_H
#define GDATA_GD_REMINDER_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_REMINDER		(gdata_gd_reminder_get_type ())
#define GDATA_GD_REMINDER(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_REMINDER, GDataGDReminder))
#define GDATA_GD_REMINDER_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_REMINDER, GDataGDReminderClass))
#define GDATA_IS_GD_REMINDER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_REMINDER))
#define GDATA_IS_GD_REMINDER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_REMINDER))
#define GDATA_GD_REMINDER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_REMINDER, GDataGDReminderClass))

typedef struct _GDataGDReminderPrivate	GDataGDReminderPrivate;

/**
 * GDataGDReminder:
 *
 * All the fields in the #GDataGDReminder structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDReminderPrivate *priv;
} GDataGDReminder;

/**
 * GDataGDReminderClass:
 *
 * All the fields in the #GDataGDReminderClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDReminderClass;

GType gdata_gd_reminder_get_type (void) G_GNUC_CONST;

GDataGDReminder *gdata_gd_reminder_new (const gchar *method, GTimeVal *absolute_time, gint relative_time) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_reminder_compare (const GDataGDReminder *a, const GDataGDReminder *b);

const gchar *gdata_gd_reminder_get_method (GDataGDReminder *self);
void gdata_gd_reminder_set_method (GDataGDReminder *self, const gchar *method);

void gdata_gd_reminder_get_absolute_time (GDataGDReminder *self, GTimeVal *absolute_time);
void gdata_gd_reminder_set_absolute_time (GDataGDReminder *self, GTimeVal *absolute_time);
gboolean gdata_gd_reminder_is_absolute_time (GDataGDReminder *self);

gint gdata_gd_reminder_get_relative_time (GDataGDReminder *self);
void gdata_gd_reminder_set_relative_time (GDataGDReminder *self, gint relative_time);

G_END_DECLS

#endif /* !GDATA_GD_REMINDER_H */
