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

#include "ibusinternal.h"
#include "ibusmarshalers.h"
#include "ibusshare.h"
#include "ibusconfig.h"

#define IBUS_CONFIG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), IBUS_TYPE_CONFIG, IBusConfigPrivate))

enum {
    VALUE_CHANGED,
    LAST_SIGNAL,
};


/* IBusConfigPriv */
struct _IBusConfigPrivate {
    gpointer pad;
};
typedef struct _IBusConfigPrivate IBusConfigPrivate;

#if 0
struct _BusPair {
    GValue car;
    GValue cdr;
};
typedef struct _BusPair BusPair;
#endif

static guint    config_signals[LAST_SIGNAL] = { 0 };

#if 0
/* functions prototype */
static BusPair  *bus_pair_new                   (GType                  car_type,
                                                 GType                  cdr_type,
                                                 gpointer               car,
                                                 gpointer               cdr);
static BusPair  *bus_pair_copy                  (BusPair                *pair);
static void      bus_pair_free                  (BusPair                *pair);
#endif
static void      ibus_config_class_init    (IBusConfigClass    *klass);
static void      ibus_config_init          (IBusConfig         *config);
static void      ibus_config_real_destroy  (IBusConfig         *config);

static gboolean ibus_config_ibus_signal    (IBusProxy           *proxy,
                                            IBusMessage         *message);

static IBusProxyClass  *parent_class = NULL;

#if 0
static BusPair *
bus_pair_new (GType     car_type,
              GType     cdr_type,
              gpointer  car,
              gpointer  cdr)
{

    g_assert (car_type == G_TYPE_STRING ||
              car_type == G_TYPE_INT ||
              car_type == G_TYPE_BOOLEAN ||
              car_type == G_TYPE_DOUBLE);
    g_assert (cdr_type == G_TYPE_STRING ||
              cdr_type == G_TYPE_INT ||
              cdr_type == G_TYPE_BOOLEAN ||
              cdr_type == G_TYPE_DOUBLE);
    g_assert (car != NULL);
    g_assert (cdr != NULL);

    BusPair *pair;

    pair = g_slice_new0 (BusPair);

    g_value_init (&(pair->car), car_type);
    g_value_init (&(pair->cdr), cdr_type);

    switch (car_type) {
    case G_TYPE_STRING:
        g_value_set_string (&(pair->car), *(gchar **)car);
        break;
    case G_TYPE_INT:
        g_value_set_int (&(pair->car), *(gint32 *)car);
        break;
    case G_TYPE_BOOLEAN:
        g_value_set_boolean (&(pair->car), *(gboolean *)car);
        break;
    case G_TYPE_DOUBLE:
        g_value_set_double (&(pair->car), *(gdouble *)car);
        break;
    }

    switch (cdr_type) {
    case G_TYPE_STRING:
        g_value_set_string (&(pair->cdr), *(gchar **)cdr);
        break;
    case G_TYPE_INT:
        g_value_set_int (&(pair->cdr), *(gint32 *)car);
        break;
    case G_TYPE_BOOLEAN:
        g_value_set_boolean (&(pair->cdr), *(gboolean *)cdr);
        break;
    case G_TYPE_DOUBLE:
        g_value_set_double (&(pair->cdr), *(gdouble *)cdr);
        break;
    }
    return pair;
}

static BusPair *
bus_pair_copy (BusPair *pair)
{
    g_assert (pair != NULL);

    BusPair *new_pair;

    new_pair = g_slice_new0 (BusPair);
    g_value_copy (&(pair->car), &(new_pair->car));
    g_value_copy (&(pair->cdr), &(new_pair->cdr));
    return new_pair;
}

static void
bus_pair_free (BusPair *pair)
{
    g_assert (pair == NULL);

    g_value_unset (&(pair->car));
    g_value_unset (&(pair->cdr));

    g_slice_free (BusPair, pair);
}

#endif

GType
ibus_config_get_type (void)
{
    static GType type = 0;

    static const GTypeInfo type_info = {
        sizeof (IBusConfigClass),
        (GBaseInitFunc)     NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)    ibus_config_class_init,
        NULL,               /* class finalize */
        NULL,               /* class data */
        sizeof (IBusConfig),
        0,
        (GInstanceInitFunc) ibus_config_init,
    };

    if (type == 0) {
        type = g_type_register_static (IBUS_TYPE_PROXY,
                    "IBusConfig",
                    &type_info,
                    (GTypeFlags)0);
    }
    return type;
}

IBusConfig *
ibus_config_new (IBusConnection *connection)
{
    g_assert (IBUS_IS_CONNECTION (connection));

    GObject *obj;
    obj = g_object_new (IBUS_TYPE_CONFIG,
                        "name", IBUS_SERVICE_CONFIG,
                        "path", IBUS_PATH_CONFIG,
                        "connection", connection,
                        NULL);

    return IBUS_CONFIG (obj);
}

static void
ibus_config_class_init (IBusConfigClass *klass)
{
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
    IBusProxyClass *proxy_class = IBUS_PROXY_CLASS (klass);


    parent_class = (IBusProxyClass *) g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof (IBusConfigPrivate));

    ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_config_real_destroy;

    proxy_class->ibus_signal = ibus_config_ibus_signal;

    /* install signals */
    config_signals[VALUE_CHANGED] =
        g_signal_new (I_("value-changed"),
            G_TYPE_FROM_CLASS (klass),
            G_SIGNAL_RUN_LAST,
            0,
            NULL, NULL,
            ibus_marshal_VOID__STRING_STRING_BOXED,
            G_TYPE_NONE,
            3,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_VALUE | G_SIGNAL_TYPE_STATIC_SCOPE);
}

static void
ibus_config_init (IBusConfig *config)
{
    IBusConfigPrivate *priv;
    priv = IBUS_CONFIG_GET_PRIVATE (config);
}

static void
ibus_config_real_destroy (IBusConfig *config)
{
    if (ibus_proxy_get_connection ((IBusProxy *) config) != NULL) {
        ibus_proxy_call ((IBusProxy *) config,
                         "Destroy",
                         G_TYPE_INVALID);
    }

    IBUS_OBJECT_CLASS(parent_class)->destroy (IBUS_OBJECT (config));
}

static void
_from_dbus_value (IBusMessageIter   *iter,
                  GValue            *value)
{
    g_assert (iter != NULL);
    g_assert (value != NULL);

    gint type;
    IBusMessageIter sub_iter;

    type = ibus_message_iter_get_arg_type (iter);
    if (type == IBUS_TYPE_VARIANT) {
        ibus_message_iter_recurse (iter, IBUS_TYPE_VARIANT, &sub_iter);
        iter = &sub_iter;
        type = ibus_message_iter_get_arg_type (iter);
    }

    switch (type) {
    case G_TYPE_STRING:
        {
            gchar *v;
            g_value_init (value, G_TYPE_STRING);
            ibus_message_iter_get_basic (iter, &v);
            g_value_set_string (value, v);
        }
        break;
    case G_TYPE_INT:
        {
            gint v;
            g_value_init (value, G_TYPE_INT);
            ibus_message_iter_get_basic (iter, &v);
            g_value_set_int (value, v);
        }
        break;
    case G_TYPE_UINT:
        {
            guint v;
            g_value_init (value, G_TYPE_UINT);
            ibus_message_iter_get_basic (iter, &v);
            g_value_set_uint (value, v);
        }
    case G_TYPE_BOOLEAN:
        {
            gboolean v;
            g_value_init (value, G_TYPE_BOOLEAN);
            ibus_message_iter_get_basic (iter, &v);
            g_value_set_boolean (value, v);
        }
        break;
    case G_TYPE_DOUBLE:
        {
            gdouble v;
            g_value_init (value, G_TYPE_DOUBLE);
            ibus_message_iter_get_basic (iter, &v);
            g_value_set_double (value, v);
        }
        break;
    default:
        if (type == IBUS_TYPE_ARRAY) {
            GValue v = { 0 };
            IBusMessageIter sub_iter;
            gint sub_type;
            GValueArray *array;


            sub_type = ibus_message_iter_get_element_type (iter);
            g_assert (sub_type == G_TYPE_STRING ||
                      sub_type == G_TYPE_INT ||
                      sub_type == G_TYPE_UINT ||
                      sub_type == G_TYPE_BOOLEAN ||
                      sub_type == G_TYPE_DOUBLE ||
                      sub_type == IBUS_TYPE_VARIANT);

            g_value_init (value, G_TYPE_VALUE_ARRAY);
            array = g_value_array_new (0);
            ibus_message_iter_recurse (iter, IBUS_TYPE_ARRAY, &sub_iter);
            while (ibus_message_iter_get_arg_type (&sub_iter) != G_TYPE_INVALID) {
                _from_dbus_value (&sub_iter, &v);
                g_value_array_append (array, &v);
                g_value_unset (&v);
            }
            g_value_take_boxed (value, array);
            break;
        }

        g_debug ("type=%s", g_type_name (type));

        g_assert_not_reached();
    }
}

static void
_to_dbus_value (IBusMessageIter *iter,
                const GValue    *value)
{
    switch (G_VALUE_TYPE (value)) {
    case G_TYPE_STRING:
        {
            const gchar *v = g_value_get_string (value);
            ibus_message_iter_append (iter,
                                      G_TYPE_STRING,
                                      &v);
        }
        break;
    case G_TYPE_INT:
        {
            gint v = g_value_get_int (value);
            ibus_message_iter_append (iter,
                                      G_TYPE_INT,
                                      &v);
        }
        break;
    case G_TYPE_UINT:
        {
            guint v = g_value_get_uint (value);
            ibus_message_iter_append (iter,
                                      G_TYPE_UINT,
                                      &v);
        }
        break;
    case G_TYPE_BOOLEAN:
        {
            gboolean v = g_value_get_boolean (value);
            ibus_message_iter_append (iter,
                                      G_TYPE_BOOLEAN,
                                      &v);
        }
        break;
    case G_TYPE_DOUBLE:
        {
            gdouble v = g_value_get_double (value);
            ibus_message_iter_append (iter,
                                      G_TYPE_DOUBLE,
                                      &v);
        }
        break;
    default:
        if (G_TYPE_VALUE_ARRAY == G_VALUE_TYPE (value)) {
            IBusMessageIter sub_iter;
            GType type = G_TYPE_INVALID;
            gint i;
            GValueArray *array = (GValueArray *)g_value_get_boxed (value);
            ibus_message_iter_open_container (iter,
                                              IBUS_TYPE_ARRAY,
                                              "v",
                                              &sub_iter);
            if (array->n_values > 0) {
                type = G_VALUE_TYPE (&array->values[0]);
                g_assert (type == G_TYPE_STRING ||
                          type == G_TYPE_INT ||
                          type == G_TYPE_UINT ||
                          type == G_TYPE_BOOLEAN ||
                          type == G_TYPE_DOUBLE);
            }
            for (i = 0; i < array->n_values; i++) {
                g_assert (type == G_VALUE_TYPE (&array->values[i]));
                _to_dbus_value (&sub_iter, &array->values[i]);
            }
            ibus_message_iter_close_container (iter, &sub_iter);
            break;
        }
        g_assert_not_reached();
    }
}

static gboolean
ibus_config_ibus_signal (IBusProxy     *proxy,
                         IBusMessage   *message)
{
    g_assert (IBUS_IS_CONFIG (proxy));
    g_assert (message != NULL);

    IBusConfig *config;
    config = IBUS_CONFIG (proxy);

    if (ibus_message_is_signal (message, IBUS_INTERFACE_CONFIG, "ValueChanged")) {
        IBusMessageIter iter;
        gchar *section;
        gchar *name;
        GValue value = { 0 };

        ibus_message_iter_init (message, &iter);

        if (!ibus_message_iter_get (&iter, G_TYPE_STRING, &section)) {
            g_warning ("Argument 1 of ValueChanged should be a string");
            return FALSE;
        }

        if (!ibus_message_iter_get (&iter, G_TYPE_STRING, &name)) {
            g_warning ("Argument 2 of ValueChanged should be a string");
            return FALSE;
        }

        _from_dbus_value (&iter, &value);

        g_signal_emit (config,
                       config_signals[VALUE_CHANGED],
                       0,
                       section,
                       name,
                       &value);

        g_signal_stop_emission_by_name (config, "ibus-signal");
        return TRUE;
    }

    return FALSE;
}

gboolean
ibus_config_get_value (IBusConfig  *config,
                       const gchar *section,
                       const gchar *name,
                       GValue      *value)
{
    g_assert (IBUS_IS_CONFIG (config));
    g_assert (section != NULL);
    g_assert (name != NULL);
    g_assert (value != NULL);

    IBusMessage *reply;
    IBusError *error;

    reply = ibus_proxy_call_with_reply_and_block ((IBusProxy *) config,
                                                  "GetValue",
                                                  -1,
                                                  &error,
                                                  G_TYPE_STRING, &section,
                                                  G_TYPE_STRING, &name,
                                                  G_TYPE_INVALID);
    if (reply == NULL) {
        g_warning ("%s: %s", error->name, error->message);
        ibus_error_free (error);
        return FALSE;
    }

    if ((error = ibus_error_new_from_message (reply)) != NULL) {
        g_warning ("%s: %s", error->name, error->message);
        ibus_error_free (error);
        ibus_message_unref (reply);
        return FALSE;
    }

    IBusMessageIter iter;
    ibus_message_iter_init (reply, &iter);
    _from_dbus_value (&iter, value);

    ibus_message_unref (reply);

    return TRUE;
}

gboolean
ibus_config_set_value (IBusConfig  *config,
                            const gchar     *section,
                            const gchar     *name,
                            const GValue    *value)
{
    g_assert (IBUS_IS_CONFIG (config));
    g_assert (section != NULL);
    g_assert (name != NULL);
    g_assert (value != NULL);

    IBusMessage *message;
    IBusMessageIter iter;

    message = ibus_message_new_method_call (
                                ibus_proxy_get_name ((IBusProxy *) config),
                                ibus_proxy_get_path ((IBusProxy *) config),
                                ibus_proxy_get_interface ((IBusProxy *) config),
                                "SetValue");
    ibus_message_iter_init_append (message, &iter);
    _to_dbus_value (&iter, value);

    ibus_proxy_send ((IBusProxy *) config, message);
    ibus_message_unref (message);

    return TRUE;
}
