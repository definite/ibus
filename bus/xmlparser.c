/* vim:set et sts=4: */
/* bus - The Input Bus
 * Copyright (C) 2008-2009 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include "xmlparser.h"

static GMarkupParser parser;

void
xml_free_node (XMLNode *node)
{
    if (node->name)
        g_free (node->name);

    if (node->text)
        g_free (node->text);
    
    if (node->attributes) {
        gchar **p;
        for (p = node->attributes; *p != NULL; p++)
            g_free (*p);
        g_free (node->attributes);
    }

    if (node->sub_nodes) {
        GSList *p;

        for (p = node->sub_nodes; p != NULL; p = p->next)
            xml_free_node (p->data);

        g_slist_free (node->sub_nodes);
    }

    g_slice_free (XMLNode, node);
}

static void
_start_root_element_cb (GMarkupParseContext *context,
                        const gchar         *element_name,
                        const gchar        **attribute_names,
                        const gchar        **attribute_values,
                        gpointer             user_data,
                        GError             **error)
{
    XMLNode **node = (XMLNode **) user_data;
    g_assert (node != NULL);
    
    XMLNode *p = g_slice_new0 (XMLNode);
    
    
    p->name = g_strdup (element_name);

    GArray *attributes = g_array_new (TRUE, TRUE, sizeof (gchar *));
    while (*attribute_names != NULL && *attribute_values != NULL) {
        gchar *p;
        p = g_strdup (*attribute_names++);
        g_array_append_val (attributes, p);
        p = g_strdup (*attribute_values++);
        g_array_append_val (attributes, p);
    }
    
    p->attributes = (gchar **)g_array_free (attributes, FALSE);
    
    g_markup_parse_context_push (context, &parser, p);
    *node = p;
}


static void
_start_element_cb (GMarkupParseContext *context,
                   const gchar         *element_name,
                   const gchar        **attribute_names,
                   const gchar        **attribute_values,
                   gpointer             user_data,
                   GError             **error)
{
    XMLNode *node = (XMLNode *) user_data;

    if (node->text) {
        g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, " ");
        return;
    }
    
    XMLNode *p = g_slice_new0 (XMLNode);
    
    node->sub_nodes = g_slist_append (node->sub_nodes, p);
    g_markup_parse_context_push (context, &parser, p);

    p->name = g_strdup (element_name);

    GArray *attributes = g_array_new (TRUE, TRUE, sizeof (gchar *));
    while (*attribute_names != NULL && *attribute_values != NULL) {
        gchar *p;
        p = g_strdup (*attribute_names++);
        g_array_append_val (attributes, p);
        p = g_strdup (*attribute_values++);
        g_array_append_val (attributes, p);
    }
    
    p->attributes = (gchar **)g_array_free (attributes, FALSE);
}

static void
_end_element_cb (GMarkupParseContext *context,
                 const gchar         *element_name,
                 gpointer             user_data,
                 GError             **error)
{
    XMLNode *p = (XMLNode *) g_markup_parse_context_pop (context);

    if (p->text && p->sub_nodes) {
        g_debug ("Error");
    }
}

static gboolean
_is_space (const gchar *text,
           gsize        text_len)
{
    gsize i = 0;

    for (i = 0; text[i] != '\0' && i < text_len; i++) {
        switch (text[i]) {
        case '\t':
        case ' ':
        case '\n':
        case '\r':
            continue;
        default:
            return FALSE;
        }
    }

    return TRUE;
}

static void
_text_cb (GMarkupParseContext *context,
          const gchar         *text,
          gsize                text_len,  
          gpointer             user_data,
          GError             **error)
{
    XMLNode *p = (XMLNode *)user_data;

    if (_is_space (text, text_len))
        return;

    if (p->sub_nodes || p->text) {
        g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT, " ");
        return;
    }

    p->text = g_strndup (text, text_len);
}

static GMarkupParser parser = {
    _start_element_cb,
    _end_element_cb,
    _text_cb,
    0,
    0,
};

XMLNode *
xml_parse_file (const gchar *filename)
{
    gboolean retval;
    GError *error = NULL;
    FILE *pf = fopen (filename, "r");
    
    GMarkupParseContext *context;
    XMLNode *node;
    
    const static GMarkupParser root_parser = {
        _start_root_element_cb,
        _end_element_cb,
        _text_cb,
        0,
        0,
    };   
    
    context = g_markup_parse_context_new (&root_parser, 0, &node, 0);
    
    while (!feof (pf)) {
        gchar buf[1024];
        gssize len = 0;

        len = fread (buf, 1, sizeof (buf), pf);
        retval = g_markup_parse_context_parse (context, buf, len, &error);

        if (!retval)
            goto _failed_out;
    }
    
    fclose (pf);

    retval = g_markup_parse_context_end_parse (context, &error);
    if (!retval)
        goto _failed_out;

    g_markup_parse_context_free (context);

    return node;

_failed_out:
    g_markup_parse_context_free (context);
    return NULL;
}

static void
output_indent (int level, GString *output)
{
    gint i;
    for (i = 0; i < level; i++) {
        g_string_append (output, "    ");
    }
}

static void
xml_output_indent (const XMLNode *node, int level, GString *output)
{
    gchar **attrs;

    output_indent (level, output);
    g_string_append_printf (output, "<%s", node->name);

    attrs = node->attributes;

    while (attrs != NULL && *attrs != NULL) {
        g_string_append_printf (output, " %s", *(attrs++));
        g_string_append_printf (output, "=\"%s\"", *(attrs++));
    }

    if (node->sub_nodes != NULL){
        g_string_append (output, ">\n");
        GSList *sub_node;

        for (sub_node = node->sub_nodes; sub_node != NULL; sub_node = sub_node->next) {
            xml_output_indent (sub_node->data, level + 1, output);
        }
        output_indent (level, output);
        g_string_append_printf (output, "</%s>\n",node->name);
    }
    else if (node->text != NULL) {
        gchar *text = g_markup_escape_text (node->text, -1);
        g_string_append_printf (output, ">%s</%s>\n", text, node->name);
        g_free (text);
    }
    else {
        g_string_append (output, "/>\n");
    }
}

void
xml_output (const XMLNode *node, GString *output)
{
    xml_output_indent (node, 0, output);    
}

