cmake_minimum_required(VERSION 3.16)
project(homegear_nodes_ui)

set(CMAKE_CXX_STANDARD 17)

add_library(homegear_nodes_ui
        ui/base/UiBase.cpp
        ui/base/UiBase.h
        ui/door/Factory.cpp
        ui/door/Factory.h
        ui/door/Door.cpp
        ui/door/Door.h
        ui/hvac/Factory.cpp
        ui/hvac/Factory.h
        ui/hvac/Hvac.cpp
        ui/hvac/Hvac.h
        ui/slider/Factory.cpp
        ui/slider/Factory.h
        ui/slider/Slider.cpp
        ui/slider/Slider.h
        ui/socket/Factory.cpp
        ui/socket/Factory.h
        ui/socket/Socket.cpp
        ui/socket/Socket.h
        ui/status/Factory.cpp
        ui/status/Factory.h
        ui/status/Status.cpp
        ui/status/Status.h
        ui/switch/Factory.cpp
        ui/switch/Factory.h
        ui/switch/Switch.cpp
        ui/switch/Switch.h
        ui/window/Factory.cpp
        ui/window/Factory.h
        ui/window/Window.cpp
        ui/window/Window.h)
