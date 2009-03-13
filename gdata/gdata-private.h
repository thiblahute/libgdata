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

#include <glib.h>
#include <libxml/parser.h>

#include <gdata/gdata-service.h>

#ifndef GDATA_PRIVATE_H
#define GDATA_PRIVATE_H

G_BEGIN_DECLS

#include "gdata-feed.h"
GDataFeed *_gdata_feed_new_from_xml (const gchar *xml, gint length, GDataEntryParserFunc parser_func,
				     GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error);

#include "gdata-entry.h"
GDataEntry *_gdata_entry_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error);
gboolean _gdata_entry_parse_xml_node (GDataEntry *self, xmlDoc *doc, xmlNode *node, GError **error);

#include "services/youtube/gdata-youtube-video.h"
GDataYouTubeVideo *_gdata_youtube_video_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error);
gboolean _gdata_youtube_video_parse_xml_node (GDataYouTubeVideo *self, xmlDoc *doc, xmlNode *node, GError **error);

#include "services/calendar/gdata-calendar-calendar.h"
GDataCalendarCalendar *_gdata_calendar_calendar_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error);
gboolean _gdata_calendar_calendar_parse_xml_node (GDataCalendarCalendar *self, xmlDoc *doc, xmlNode *node, GError **error);

#include "services/calendar/gdata-calendar-event.h"
GDataCalendarEvent *_gdata_calendar_event_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error);
gboolean _gdata_calendar_event_parse_xml_node (GDataCalendarEvent *self, xmlDoc *doc, xmlNode *node, GError **error);

G_END_DECLS

#endif /* !GDATA_PRIVATE_H */
