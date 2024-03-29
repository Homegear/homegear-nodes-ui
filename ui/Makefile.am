AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++17 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear
LIBS += -Wl,-Bdynamic -lhomegear-node

libdir = $(localstatedir)/lib/homegear/node-blue/nodes/ui

lib_LTLIBRARIES = ui-button.la ui-door.la ui-hvac.la ui-light.la ui-shading.la ui-slider.la ui-socket.la ui-status.la ui-switch.la ui-window.la

ui_button_la_SOURCES = base/UiBase.cpp button/Factory.cpp button/Button.cpp
ui_button_la_LDFLAGS = -module -avoid-version -shared

ui_door_la_SOURCES = base/UiBase.cpp door/Factory.cpp door/Door.cpp
ui_door_la_LDFLAGS = -module -avoid-version -shared

ui_hvac_la_SOURCES = base/UiBase.cpp hvac/Factory.cpp hvac/Hvac.cpp
ui_hvac_la_LDFLAGS = -module -avoid-version -shared

ui_light_la_SOURCES = base/UiBase.cpp light/Factory.cpp light/Light.cpp
ui_light_la_LDFLAGS = -module -avoid-version -shared

ui_shading_la_SOURCES = base/UiBase.cpp shading/Factory.cpp shading/Shading.cpp
ui_shading_la_LDFLAGS = -module -avoid-version -shared

ui_slider_la_SOURCES = base/UiBase.cpp slider/Factory.cpp slider/Slider.cpp
ui_slider_la_LDFLAGS = -module -avoid-version -shared

ui_socket_la_SOURCES = base/UiBase.cpp socket/Factory.cpp socket/Socket.cpp
ui_socket_la_LDFLAGS = -module -avoid-version -shared

ui_status_la_SOURCES = base/UiBase.cpp status/Factory.cpp status/Status.cpp
ui_status_la_LDFLAGS = -module -avoid-version -shared

ui_switch_la_SOURCES = base/UiBase.cpp switch/Factory.cpp switch/Switch.cpp
ui_switch_la_LDFLAGS = -module -avoid-version -shared

ui_window_la_SOURCES = base/UiBase.cpp window/Factory.cpp window/Window.cpp
ui_window_la_LDFLAGS = -module -avoid-version -shared

ui_ladir = $(libdir)
ui_la_DATA = package.json button/ui-button.html door/ui-door.html hvac/ui-hvac.html light/ui-light.html shading/ui-shading.html slider/ui-slider.html socket/ui-socket.html status/ui-status.html switch/ui-switch.html window/ui-window.html

locale_endir = $(libdir)/locales/en
locale_en_DATA = locales/en-US/ui.json

locale_en_usdir = $(libdir)/locales/en-US
locale_en_us_DATA = locales/en-US/ui.json

locale_dedir = $(libdir)/locales/de
locale_de_DATA = locales/de-DE/ui.json

locale_de_dedir = $(libdir)/locales/de-DE
locale_de_de_DATA = locales/de-DE/ui.json

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/ui-button.la
	rm -f $(DESTDIR)$(libdir)/ui-door.la
	rm -f $(DESTDIR)$(libdir)/ui-hvac.la
	rm -f $(DESTDIR)$(libdir)/ui-light.la
	rm -f $(DESTDIR)$(libdir)/ui-shading.la
	rm -f $(DESTDIR)$(libdir)/ui-slider.la
	rm -f $(DESTDIR)$(libdir)/ui-socket.la
	rm -f $(DESTDIR)$(libdir)/ui-status.la
	rm -f $(DESTDIR)$(libdir)/ui-switch.la
	rm -f $(DESTDIR)$(libdir)/ui-window.la
