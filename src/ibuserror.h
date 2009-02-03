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
#ifndef __IBUS_ERROR_H_
#define __IBUS_ERROR_H_

#include <glib.h>
#include <dbus/dbus.h>

G_BEGIN_DECLS

typedef DBusError IBusError;

IBusError       *ibus_error_new             (void);
IBusError       *ibus_error_new_from_text   (const gchar    *name,
                                             const gchar    *message);
IBusError       *ibus_error_new_from_printf (const gchar    *name,
                                             const gchar    *format_message,
                                             ...);
IBusError       *ibus_error_new_from_message
                                            (DBusMessage    *message);
void             ibus_error_free            (IBusError      *error);

G_END_DECLS
#endif
