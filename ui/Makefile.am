AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++17 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear
LIBS += -Wl,-Bdynamic -lhomegear-node

libdir = $(localstatedir)/lib/homegear/node-blue/nodes/ui

lib_LTLIBRARIES = ui-heating.la ui-socket.la

ui_heating_la_SOURCES = base/UiBase.cpp heating/Factory.cpp heating/Heating.cpp
ui_heating_la_LDFLAGS =-module -avoid-version -shared

ui_socket_la_SOURCES = base/UiBase.cpp socket/Factory.cpp socket/Socket.cpp
ui_socket_la_LDFLAGS =-module -avoid-version -shared

ui_ladir = $(libdir)
ui_la_DATA = package.json heating/ui-heating.html socket/ui-socket.html
locale_en_usdir = $(libdir)/locales/en-US
locale_en_us_DATA = locales/en-US/ui.json
locale_de_dedir = $(libdir)/locales/de-DE
locale_de_de_DATA = locales/de-DE/ui.json

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/ui-heating.la
	rm -f $(DESTDIR)$(libdir)/ui-socket.la
