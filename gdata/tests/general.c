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

#include "gdata.h"

static void
test_entry_get_xml (void)
{
	GTimeVal updated, published, updated2, published2;
	GDataEntry *entry, *entry2;
	GDataCategory *category;
	GDataLink *link;
	GDataAuthor *author;
	gchar *xml;
	GError *error = NULL;

	entry = gdata_entry_new ();
	gdata_entry_set_title (entry, "Testing title & escaping");
	gdata_entry_set_id (entry, "tag:youtube,2008:video:24Ryj1ywosw");
	gdata_entry_set_content (entry, "This is some sample content testing, amongst other things, <markup> & odd characters‽");

	g_time_val_from_iso8601 ("2009-01-25T14:07:37.880860Z", &updated);
	gdata_entry_set_updated (entry, &updated);

	g_time_val_from_iso8601 ("2009-01-23T14:06:37.880860Z", &published);
	gdata_entry_set_published (entry, &published);

	/* Categories */
	category = gdata_category_new ("test", NULL, NULL);
	gdata_entry_add_category (entry, category);
	category = gdata_category_new ("example", NULL, "Example stuff");
	gdata_entry_add_category (entry, category);
	category = gdata_category_new ("Film", "http://gdata.youtube.com/schemas/2007/categories.cat", "Film & Animation");
	gdata_entry_add_category (entry, category);

	/* Links */
	link = gdata_link_new ("http://test.com/", "self", "application/atom+xml", NULL, NULL, -1);
	gdata_entry_add_link (entry, link);
	link = gdata_link_new ("http://example.com/", NULL, NULL, NULL, NULL, -1);
	gdata_entry_add_link (entry, link);
	link = gdata_link_new ("http://test.mn/", "related", "text/html", "mn", "A treatise on Mongolian test websites & other stuff.", 5010);
	gdata_entry_add_link (entry, link);

	/* Authors */
	author = gdata_author_new ("Joe Bloggs", "http://example.com/", "joe@example.com");
	gdata_entry_add_author (entry, author);
	author = gdata_author_new ("John Smith", NULL, "smith.john@example.com");
	gdata_entry_add_author (entry, author);
	author = gdata_author_new ("F. Barr؟", NULL, NULL);
	gdata_entry_add_author (entry, author);

	/* Check the generated XML's OK */
	xml = gdata_entry_get_xml (entry);
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' >"
				 "<title type='text'>Testing title &amp; escaping</title>"
				 "<id>tag:youtube,2008:video:24Ryj1ywosw</id>"
				 "<updated>2009-01-25T14:07:37.880860Z</updated>"
				 "<published>2009-01-23T14:06:37.880860Z</published>"
				 "<content type='text'>This is some sample content testing, amongst other things, &lt;markup&gt; &amp; odd characters\342\200\275</content>"
				 "<category term='Film' scheme='http://gdata.youtube.com/schemas/2007/categories.cat' label='Film &amp; Animation'/>"
				 "<category term='example' label='Example stuff'/>"
				 "<category term='test'/>"
				 "<link href='http://test.mn/' title='A treatise on Mongolian test websites &amp; other stuff.' rel='related' type='text/html' hreflang='mn' length='5010'/>"
				 "<link href='http://example.com/'/>"
				 "<link href='http://test.com/' rel='self' type='application/atom+xml'/>"
				 "<author><name>F. Barr\330\237</name></author>"
				 "<author><name>John Smith</name><email>smith.john@example.com</email></author>"
				 "<author><name>Joe Bloggs</name><uri>http://example.com/</uri><email>joe@example.com</email></author>"
			 "</entry>");

	/* Check again by re-parsing the XML to a GDataEntry */
	entry2 = gdata_entry_new_from_xml (xml, -1, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (entry2));
	g_clear_error (&error);
	g_free (xml);

	g_assert_cmpstr (gdata_entry_get_title (entry), ==, gdata_entry_get_title (entry2));
	g_assert_cmpstr (gdata_entry_get_id (entry), ==, gdata_entry_get_id (entry2));
	g_assert_cmpstr (gdata_entry_get_content (entry), ==, gdata_entry_get_content (entry2));

	gdata_entry_get_updated (entry, &updated);
	gdata_entry_get_updated (entry2, &updated2);
	g_assert_cmpuint (updated.tv_sec, ==, updated2.tv_sec);
	g_assert_cmpuint (updated.tv_usec, ==, updated2.tv_usec);

	gdata_entry_get_published (entry, &published);
	gdata_entry_get_published (entry2, &published2);
	g_assert_cmpuint (published.tv_sec, ==, published2.tv_sec);
	g_assert_cmpuint (published.tv_usec, ==, published2.tv_usec);

	/* TODO: Check categories, links and authors */

	g_object_unref (entry);
	g_object_unref (entry2);
}

int
main (int argc, char *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/entry/get_xml", test_entry_get_xml);

	return g_test_run ();
}
