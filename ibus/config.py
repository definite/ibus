# vim:set et sts=4 sw=4:
#
# ibus - The Input Bus
#
# Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA  02111-1307  USA

__all__ = (
        "ConfigBase",
        "IBUS_SERVICE_CONFIG",
        "IBUS_PATH_CONFIG"
    )

IBUS_SERVICE_CONFIG = "org.freedesktop.IBus.Config"
IBUS_PATH_CONFIG = "/org/freedesktop/IBus/Config"

import ibus
from ibus import interface

class ConfigBase(ibus.Object):
    def __init__(self, bus):
        super(ConfigBase, self).__init__()
        self.__proxy = ConfigProxy(self, bus.get_dbusconn())

    def get_value(self, section, name):
        pass

    def set_value(self, section, name, value):
        pass

    def value_changed(self, section, name, value):
        self.__proxy.ValueChanged(section, name, value)


class ConfigProxy(interface.IConfig):
    def __init__ (self, config, dbusconn):
        super(ConfigProxy, self).__init__(dbusconn, IBUS_PATH_CONFIG)
        self.__dbusconn = dbusconn
        self.__config = config

    def GetValue(self, section, name):
        return self.__config.get_value(section, name)

    def SetValue(self, section, name, value):
        return self.__config.set_value(section, name, value)

    def Destroy(self):
        self.__config.destroy()

class Config(ibus.Object):
    def __init__(self, bus):
        self.__config = bus.get_dbusconn().get_object(IBUS_SERVICE_CONFIG, IBUS_PATH_CONFIG)

    def get_value(self, section, name, default_value):
        try:
            return self.__config.GetValue(section, name)
        except:
            return default_value

    def set_value(self, section, name, value):
        return self.__config.SetValue(section, name, value)

    def set_list(self, section, name, value, signature):
        return self.__config.SetValue(section, name, dbus.Array(value, signature=signature))
