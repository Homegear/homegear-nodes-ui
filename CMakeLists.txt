cmake_minimum_required(VERSION 3.16)
project(homegear_nodes_ui)

set(CMAKE_CXX_STANDARD 17)

add_executable(homegear_nodes_ui
        ui/base/UiBase.cpp
        ui/base/UiBase.h
        ui/heating/Factory.cpp
        ui/heating/Factory.h
        ui/heating/Heating.cpp
        ui/heating/Heating.h
        ui/socket/Factory.cpp
        ui/socket/Factory.h
        ui/socket/Socket.cpp
        ui/socket/Socket.h)
