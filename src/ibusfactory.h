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
#ifndef __IBUS_FACTORY_H_
#define __IBUS_FACTORY_H_

#include <dbus/dbus.h>
#include "ibusservice.h"
#include "ibusserializable.h"

/*
 * Type macros.
 */

/* define GOBJECT macros */
#define IBUS_TYPE_FACTORY               \
    (ibus_factory_get_type ())
#define IBUS_FACTORY(obj)               \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), IBUS_TYPE_FACTORY, IBusFactory))
#define IBUS_FACTORY_CLASS(klass)       \
    (G_TYPE_CHECK_CLASS_CAST ((klass), IBUS_TYPE_FACTORY, IBusFactoryClass))
#define IBUS_IS_FACTORY(obj)            \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IBUS_TYPE_FACTORY))
#define IBUS_IS_FACTORY_CLASS(klass)    \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), IBUS_TYPE_FACTORY))
#define IBUS_FACTORY_GET_CLASS(obj)     \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), IBUS_TYPE_FACTORY, IBusFactoryClass))

#define IBUS_TYPE_FACTORY_INFO              \
    (ibus_factory_info_get_type ())
#define IBUS_FACTORY_INFO(obj)              \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), IBUS_TYPE_FACTORY_INFO, IBusFactoryInfo))
#define IBUS_FACTORY_INFO_CLASS(klass)      \
    (G_TYPE_CHECK_CLASS_CAST ((klass), IBUS_TYPE_FACTORY_INFO, IBusFactoryInfoClass))
#define IBUS_IS_FACTORY_INFO(obj)           \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IBUS_TYPE_FACTORY_INFO))
#define IBUS_IS_FACTORY_INFO_CLASS(klass)   \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), IBUS_TYPE_FACTORY_INFO))
#define IBUS_FACTORY_INFO_GET_CLASS(obj)   \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), IBUS_TYPE_FACTORY_INFO, IBusFactoryInfoClass))


G_BEGIN_DECLS

typedef struct _IBusFactory IBusFactory;
typedef struct _IBusFactoryClass IBusFactoryClass;
typedef struct _IBusFactoryInfo IBusFactoryInfo;
typedef struct _IBusFactoryInfoClass IBusFactoryInfoClass;

struct _IBusFactory {
    IBusService parent;

    /* instance members */
};

struct _IBusFactoryClass {
    IBusServiceClass parent;

    /* signals */

    /*< private >*/
    /* padding */
    gpointer pdummy[8];
};

struct _IBusFactoryInfo {
    IBusSerializable parent;

    /* instance members */
    gchar *path;
    gchar *name;
    gchar *lang;
    gchar *icon;
    gchar *authors;
    gchar *credits;
};

struct _IBusFactoryInfoClass {
    IBusSerializableClass parent;

    /* signals */

    /*< private >*/
    /* padding */
    gpointer pdummy[8];
};

GType            ibus_factory_get_type          (void);
IBusFactory     *ibus_factory_new               (IBusConnection *connection);
void             ibus_factory_add_engine        (IBusFactory    *factory,
                                                 const gchar    *engine_name,
                                                 GType           engine_type);
IBusFactoryInfo *ibus_factory_get_info          (IBusFactory    *factory);
GType            ibus_factory_info_get_type     (void);
IBusFactoryInfo *ibus_factory_info_new          (const gchar    *path,
                                                 const gchar    *name,
                                                 const gchar    *lang,
                                                 const gchar    *icon,
                                                 const gchar    *authors,
                                                 const gchar    *credits);

G_END_DECLS
#endif

