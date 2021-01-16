AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++17 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear
LIBS += -Wl,-Bdynamic -lhomegear-node

libdir = $(localstatedir)/lib/homegear/node-blue/nodes/ui

lib_LTLIBRARIES = ui-hvac.la ui-socket.la

ui_hvac_la_SOURCES = base/UiBase.cpp hvac/Factory.cpp hvac/Hvac.cpp
ui_hvac_la_LDFLAGS =-module -avoid-version -shared

ui_socket_la_SOURCES = base/UiBase.cpp socket/Factory.cpp socket/Socket.cpp
ui_socket_la_LDFLAGS =-module -avoid-version -shared

ui_ladir = $(libdir)
ui_la_DATA = package.json hvac/ui-hvac.html socket/ui-socket.html

locale_endir = $(libdir)/locales/en
locale_en_DATA = locales/en-US/ui.json

locale_en_usdir = $(libdir)/locales/en-US
locale_en_us_DATA = locales/en-US/ui.json

locale_dedir = $(libdir)/locales/de
locale_de_DATA = locales/de-DE/ui.json

locale_de_dedir = $(libdir)/locales/de-DE
locale_de_de_DATA = locales/de-DE/ui.json

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/ui-hvac.la
	rm -f $(DESTDIR)$(libdir)/ui-socket.la
