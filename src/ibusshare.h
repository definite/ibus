/* vim:set et sts=4: */
/* ibus - The Input Bus
 * Copyright (C) 2008-2009 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __IBUS_SHARE_H_
#define __IBUS_SHARE_H_

#include <glib.h>

#define IBUS_SERVICE_IBUS       "org.freedesktop.IBus"
#define IBUS_SERVICE_PANEL      "org.freedesktop.IBus.Panel"
#define IBUS_SERVICE_CONFIG     "org.freedesktop.IBus.Config"
#define IBUS_SERVICE_NOTIFICATIONS    "org.freedesktop.IBus.Notifications"

#define IBUS_PATH_IBUS          "/org/freedesktop/IBus"
#define IBUS_PATH_FACTORY       "/org/freedesktop/IBus/Factory"
#define IBUS_PATH_PANEL         "/org/freedesktop/IBus/Panel"
#define IBUS_PATH_CONFIG        "/org/freedesktop/IBus/Config"
#define IBUS_PATH_NOTIFICATIONS "/org/freedesktop/IBus/Notifications"
#define IBUS_PATH_INPUT_CONTEXT "/org/freedesktop/IBus/InputContext_%d"

#define IBUS_INTERFACE_IBUS     "org.freedesktop.IBus"
#define IBUS_INTERFACE_INPUT_CONTEXT \
                                "org.freedesktop.IBus.InputContext"
#define IBUS_INTERFACE_FACTORY  "org.freedesktop.IBus.Factory"
#define IBUS_INTERFACE_ENGINE   "org.freedesktop.IBus.Engine"
#define IBUS_INTERFACE_PANEL    "org.freedesktop.IBus.Panel"
#define IBUS_INTERFACE_CONFIG   "org.freedesktop.IBus.Config"
#define IBUS_INTERFACE_NOTIFICATIONS    "org.freedesktop.IBus.Notifications"

G_BEGIN_DECLS

const gchar     *ibus_get_address       (void);
const gchar     *ibus_get_user_name     (void);
const gchar     *ibus_get_socket_path   (void);

const gchar     *ibus_keyval_name       (guint           keyval);
guint            ibus_keyval_from_name  (const gchar    *keyval_name);
void             ibus_free_strv         (gchar          **strv);
const gchar     *ibus_key_event_to_string
                                        (guint           keyval,
                                         guint           modifiers);

gboolean         ibus_key_event_from_string
                                        (const gchar    *string,
                                         guint          *keyval,
                                         guint          *modifiers);
void             ibus_init              (void);
void             ibus_main              (void);
void             ibus_quit              (void);

G_END_DECLS
#endif
