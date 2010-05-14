PROJECT(TrollEdit)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Point Windows to lua dir and install dll 
IF(WIN32)
	SET(LUA_LIBRARIES lua/lua5.1.dll)
	SET(LUA_INCLUDE_DIR lua)
	INSTALL(FILES ${LUA_LIBRARIES} DESTINATION .)
ENDIF()

# Find dependencies
FIND_PACKAGE(Qt4 REQUIRED)

# Setup environment
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(src/lua-5.1.4/src ${QT_INCLUDE_DIR})

# Build lua
IF(WIN32 AND NOT CYGWIN)
ELSE()
	ADD_DEFINITIONS(-DLUA_USE_POSIX -DLUA_USE_DLOPEN)
	SET(LUA_LIBS m dl)
ENDIF()

SET(LUA_SRC 
	src/lua-5.1.4/src/lapi.c 
	src/lua-5.1.4/src/lcode.c 
	src/lua-5.1.4/src/ldebug.c 
	src/lua-5.1.4/src/ldo.c 
	src/lua-5.1.4/src/ldump.c 
	src/lua-5.1.4/src/lfunc.c 
	src/lua-5.1.4/src/lgc.c 
	src/lua-5.1.4/src/llex.c 
	src/lua-5.1.4/src/lmem.c 
	src/lua-5.1.4/src/lobject.c 
	src/lua-5.1.4/src/lopcodes.c 
	src/lua-5.1.4/src/lparser.c 
	src/lua-5.1.4/src/lstate.c 
	src/lua-5.1.4/src/lstring.c 
	src/lua-5.1.4/src/ltable.c 
	src/lua-5.1.4/src/ltm.c 
	src/lua-5.1.4/src/lundump.c 
	src/lua-5.1.4/src/lvm.c 
	src/lua-5.1.4/src/lzio.c 
	src/lua-5.1.4/src/lauxlib.c 
	src/lua-5.1.4/src/lbaselib.c 
	src/lua-5.1.4/src/ldblib.c 
	src/lua-5.1.4/src/liolib.c 
	src/lua-5.1.4/src/lmathlib.c 
	src/lua-5.1.4/src/loslib.c 
	src/lua-5.1.4/src/ltablib.c 
	src/lua-5.1.4/src/lstrlib.c 
	src/lua-5.1.4/src/loadlib.c 
	src/lua-5.1.4/src/linit.c)

ADD_LIBRARY(lua ${LUA_SRC})
TARGET_LINK_LIBRARIES(lua ${LUA_LIBS})

INCLUDE_DIRECTORIES ( src )

# Build lpeg
ADD_LIBRARY(lpeg src/lpeg-0.9/lpeg.c)

# Collect sources, headers and resources
FILE(GLOB_RECURSE TROLL_CPP src/*.cpp)
FILE(GLOB_RECURSE TROLL_H src/*.h)
FILE(GLOB_RECURSE TROLL_RC res/*.qrc)
    
# Process resources with Qt4 tools 
QT4_ADD_RESOURCES(TROLL_RC ${TROLL_RC})
QT4_WRAP_CPP(TROLL_MOC ${TROLL_H})

# Build TrollEdit, include lpeg into it.
ADD_EXECUTABLE(trolledit ${TROLL_CPP} ${TROLL_MOC} ${TROLL_RC})
TARGET_LINK_LIBRARIES(trolledit ${QT_LIBRARIES} lua lpeg)

# Install
INSTALL(TARGETS trolledit DESTINATION .)
INSTALL(DIRECTORY data/ DESTINATION .)

