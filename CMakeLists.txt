project(trolledit) 
cmake_minimum_required(VERSION 2.8.0)

## INSTALL DEFAULTS (Relative to CMAKE_INSTALL_PREFIX)
set ( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set ( INSTALL_LIB lib CACHE PATH "Where to install libraries to." )
set ( INSTALL_INC include CACHE PATH "Where to install headers to." )
set ( INSTALL_ETC etc CACHE PATH "Where to store configuration files" )
set ( INSTALL_LMOD ${INSTALL_LIB}/lua CACHE PATH "Directory to install Lua modules." )
set ( INSTALL_CMOD ${INSTALL_LIB}/lua CACHE PATH "Directory to install Lua binary modules." )
set ( INSTALL_DATA share/trolledit CACHE PATH "Directory for shared data." )

# Settings
option(USE_BUILTIN_LUA "Use builtin LuaJIT2" ON)

# Override default install path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/TrollEdit CACHE PATH "Installation Destination" FORCE)
endif()

# Apple compilation fix
if (APPLE)
  set ( CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000" )
endif ()

# Build lua and lpeg libs if needed
if (USE_BUILTIN_LUA)
  set ( LUA_LIBRARY liblua )
  set ( LUA_INCLUDE_DIR luajit/src )
  include_directories(${LUA_INCLUDE_DIR})
  add_subdirectory(luajit)
  add_subdirectory(lpeg)
else()
  # Find HOST Lua implementation
  find_package(Lua51 REQUIRED)
  include_directories(${LUA_INCLUDE_DIR})
endif()

# Find dependencies
find_package(Qt4 REQUIRED)

# Setup environment
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})

# Collect sources, headers and resources
file(GLOB_RECURSE TROLL_CPP src/*.cpp)
file(GLOB_RECURSE TROLL_H src/*.h)
file(GLOB_RECURSE TROLL_RC res/*.qrc)
    
# Process resources with Qt4 tools 
qt4_add_resources(TROLL_RC ${TROLL_RC})
qt4_wrap_cpp(TROLL_MOC ${TROLL_H})

# Build TrollEdit, include lpeg into it.
add_executable(trolledit ${TROLL_CPP} ${TROLL_MOC} ${TROLL_RC})
target_link_libraries(trolledit ${QT_LIBRARIES} ${LUA_LIBRARY})

# Install
install(TARGETS trolledit DESTINATION ${INSTALL_BIN})
install(DIRECTORY data/ DESTINATION ${INSTALL_DATA})

