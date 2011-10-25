# Copyright (C) 2011 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project(lpeg C)
cmake_minimum_required(VERSION 2.8)

find_package(Lua51 REQUIRED)

add_library(lpeg MODULE lpeg.c)
target_link_libraries(lpeg ${LUA_LIBRARY})
set_target_properties(lpeg PROPERTIES PREFIX "")

install(TARGETS lpeg DESTINATION ${INSTALL_CMOD})
install(FILES re.lua DESTINATION ${INSTALL_LMOD})
