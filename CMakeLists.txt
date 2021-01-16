cmake_minimum_required(VERSION 3.16)
project(homegear_nodes_ui)

set(CMAKE_CXX_STANDARD 17)

add_library(homegear_nodes_ui
        ui/base/UiBase.cpp
        ui/base/UiBase.h
        ui/hvac/Factory.cpp
        ui/hvac/Factory.h
        ui/hvac/Hvac.cpp
        ui/hvac/Hvac.h
        ui/socket/Factory.cpp
        ui/socket/Factory.h
        ui/socket/Socket.cpp
        ui/socket/Socket.h)
