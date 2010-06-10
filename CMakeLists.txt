PROJECT(TrollEdit) 
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Settings
OPTION(USE_JIT "Use LuaJIT instead of Lua" TRUE)

# Override default install path
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/TrollEdit CACHE PATH "Installation Destination" FORCE)
ENDIF()

# Default Install prefix

# Build lua and lpeg libs
IF (USE_JIT)
	INCLUDE_DIRECTORIES(src/luajit-1.1.6/src)
	SUBDIRS(src/luajit-1.1.6)
ELSE()
	INCLUDE_DIRECTORIES(src/lua-5.1.4/src)
	SUBDIRS(src/lua-5.1.4)
ENDIF()
SUBDIRS(src/lpeg-0.9)

# Find dependencies
FIND_PACKAGE(Qt4 REQUIRED)

# Setup environment
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

# Collect sources, headers and resources
FILE(GLOB_RECURSE TROLL_CPP src/trolledit/*.cpp)
FILE(GLOB_RECURSE TROLL_H src/trolledit/*.h)
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

