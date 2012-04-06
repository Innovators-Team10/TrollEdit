project(TrollEdit CXX)
cmake_minimum_required(VERSION 2.8)

# -------------
# Install paths
# -------------
set ( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set ( INSTALL_DATA share/trolledit CACHE PATH "Directory for shared data." )

# ------------------
# TrollEdit Settings
# ------------------
set(TROLLEDIT_NAME "TrollEdit")
set(TROLLEDIT_VERSION_MAJOR "0")
set(TROLLEDIT_VERSION_MINOR "1")
set(TROLLEDIT_VERSION_PATCH "0")
set(TROLLEDIT_VERSION "${TROLLEDIT_VERSION_MAJOR}.${TROLLEDIT_VERSION_MINOR}.${TROLLEDIT_VERSION_PATCH}")
set(TROLLEDIT_VENDOR "TrollEdit Dev Team")
set(TROLLEDIT_COPYRIGHT_YEAR "2012")
set(TROLLEDIT_DOMAIN_FIRST "trolledit")
set(TROLLEDIT_DOMAIN_SECOND "org")
set(TROLLEDIT_DOMAIN "${TROLLEDIT_DOMAIN_FIRST}.${TROLLEDIT_DOMAIN_SECOND}")

# --------------
# Build Settings
# --------------

# Warnings for Debug mode
if(CMAKE_COMPILER_IS_GNUCXX OR APPLE)
        set(CMAKE_CXX_FLAGS_DEBUG -Wall -Wextra -Werror)
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Default install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/TrollEdit CACHE PATH "Installation Destination" FORCE)
endif()

# ------------
# Dependencies
# ------------

# Build lua and lpeg libs if needed
option(USE_BUILTIN_LUA "Use builtin LuaJIT2" ON)
if(USE_BUILTIN_LUA)
  # LuaJIT2 specific build settings on Apple
  if(APPLE)
    set( CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000" )
  endif()

  # Set search paths to the dependencies
  set ( DEP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies )
  set ( DEP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dependencies )
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEP_BIN}/lib ${DEP_BIN}/bin)
  set(CMAKE_INCLUDE_PATH ${CMaKE_INCLUDE_PATH} ${DEP_BIN}/include)

  # Build LuaJIT2
  include ( ExternalProject )
  ExternalProject_Add( dep_luajit
      PREFIX luajit
      SOURCE_DIR ${DEP_SOURCE}/luajit
      BINARY_DIR luajit
      CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}

    )
    
  # Build lpeg
  ExternalProject_Add( dep_lpeg
      PREFIX lpeg
      SOURCE_DIR ${DEP_SOURCE}/lpeg
      BINARY_DIR lpeg
      CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
    )
    
  # Ensure correct order of build
  add_dependencies ( dep_lpeg dep_luajit )
  
  # Define LUA_ variables
  set(LUA_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX})
  if (WIN32)
    set(LUA_LIBRARY ${DEP_BIN}/bin/liblua.dll)    
  endif()
  set(LUA_INCLUDE_DIR ${DEP_BIN}/include)
else()
  # Find Lua implementation, assuming lpeg is installed by other means
  find_package(Lua51 REQUIRED)
  
  # RPath functionality (relative paths to libraries)
  set( CMAKE_SKIP_BUILD_RPATH FALSE )
  set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
  set( CMAKE_INSTALL_RPATH $ORIGIN/../lib )
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
  set( CMAKE_INSTALL_NAME_DIR @executable_path/../lib )
endif()

# Find QT4
find_package(Qt4 COMPONENTS QtMain QtCore QtGui REQUIRED)
include(${QT_USE_FILE})
set(CMAKE_AUTOMOC TRUE)

# ---------------
# Build TrollEdit
# ---------------

# Apple specific overrides
if (APPLE)
  option(TROLLEDIT_BUILD_BUNDLE "Build Trolledit as Bundle" OFF)
  if(TROLLEDIT_BUILD_BUNDLE)
    # Executable settings
    set(TROLLEDIT_EXECUTABLE_TYPE MACOSX_BUNDLE)
    set(BUNDLE_APP "${CMAKE_INSTALL_PREFIX}/${TROLLEDIT_NAME}.app")
    
    # Override default install destinations
    set(INSTALL_BUNDLE .)
    set(INSTALL_BIN ${TROLLEDIT_NAME}.app/Contents/MacOS)
    set(INSTALL_DATA ${TROLLEDIT_NAME}.app/Contents/Resources)
  
    # Bundle settings
    set(MACOSX_BUNDLE_INFO_STRING "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${TROLLEDIT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "${TROLLEDIT_COPYRIGHT_YEAR} ${TROLLEDIT_VENDOR}")
    set(MACOSX_BUNDLE_ICON_FILE "trolledit.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${TROLLEDIT_DOMAIN_SECOND}.${TROLLEDIT_DOMAIN_FIRST}.${TROLLEDIT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${TROLLEDIT_NAME}")
    
    # CPack Settings  
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/trolledit.icns")
    set(CPACK_PACKAGE_FILE_NAME "${TROLLEDIT_NAME}-${TROLLEDIT_VERSION}")
    
    # OSX Specific resurces
    install(FILES res/trolledit.icns DESTINATION ${INSTALL_DATA})
  endif()
endif()

# Windows specific overrides
if(WIN32)
  # Executable settings
  set(TROLLEDIT_EXECUTABLE_TYPE WIN32)
  set(BUNDLE_APP "${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/${TROLLEDIT_NAME}.exe")
  
  # CPack settings
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_EXECUTABLES "${TROLLEDIT_NAME}" "${TROLLEDIT_NAME}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${TROLLEDIT_NAME}")
  set(CPACK_PACKAGE_FILE_NAME "${TROLLEDIT_NAME}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/${TROLLEDIT_LICENSE}")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${INSTALL_BIN}")
  set(CPACK_NSIS_MUI_ICON "res/trolledit.ico")
  set(CPACK_PACKAGE_ICON "res\\\\trolledit.ico")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://${TROLLEDIT_DOMAIN}")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${TROLLEDIT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
  set(CPACK_NSIS_MENU_LINKS "${TROLLEDIT_LICENSE}" "${TROLLEDIT_README}")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_NSIS_INSTALLED_ICON_NAME}")
endif()

# Definitions
add_definitions(-DTROLLEDIT_VERSION=\"${TROLLEDIT_VERSION}\")

# Include Paths
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LUA_INCLUDE_DIR})

# Files
file(GLOB_RECURSE TROLLEDIT_SRC src/*.cpp)
file(GLOB_RECURSE TROLLEDIT_UI ui/*.ui)
file(GLOB_RECURSE TROLLEDIT_QRC res/*.qrc )
file(GLOB_RECURSE TROLLEDIT_RC res/*.rc )
set(TROLLEDIT_LICENSE "LICENSE.txt")
set(TROLLEDIT_README "README.md")

# Generators
qt4_wrap_ui(TROLLEDIT_UI_GEN ${TROLLEDIT_UI})
qt4_add_resources(TROLLEDIT_QRC_GEN ${TROLLEDIT_QRC})

# Build and link
add_executable(${TROLLEDIT_NAME} ${TROLLEDIT_EXECUTABLE_TYPE} 
  ${TROLLEDIT_SRC}
  ${TROLLEDIT_UI_GEN}
  ${TROLLEDIT_QRC_GEN}
  ${TROLLEDIT_RC})
target_link_libraries(${TROLLEDIT_NAME} ${QT_LIBRARIES} ${LUA_LIBRARY})

# Make sure dependencies are built first
if(USE_BUILTIN_LUA)
  add_dependencies(${TROLLEDIT_NAME} dep_luajit)
endif()
  
# -------
# Install
# -------

# Trolledit and data
install(TARGETS ${TROLLEDIT_NAME} 
  BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
  RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime
  )
install(FILES ${TROLLEDIT_LICENSE} ${TROLLEDIT_README} 
  DESTINATION ${INSTALL_DATA})
install(DIRECTORY data/ 
  DESTINATION ${INSTALL_DATA})

# Bundle QT4 on Windows and Apple
if (WIN32 OR TROLLEDIT_BUILD_BUNDLE)
  # Install Lua modules
  install(DIRECTORY ${DEP_BIN}/lib/lua 
    DESTINATION ${INSTALL_DATA} )

  # Bundle libraries
  #SET(BUNDLE_LIBS "${QT_LIBRARIES}" "${LUA_LIBRARY}")
  set(BUNDLE_DIRS "${QT_LIBRARY_DIR}" "${DEP_BIN}/bin" "${DEP_BIN}/lib" )
  install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"${BUNDLE_APP}\" \"${BUNDLE_LIBS}\" \"${BUNDLE_DIRS}\")
      " COMPONENT Runtime)
elseif(USE_BUILTIN_LUA)
  # Install all dependencies
  install(DIRECTORY ${DEP_BIN}/ 
    DESTINATION . 
    USE_SOURCE_PERMISSIONS)
endif()

# -------
# Packing
# -------

include(CPack)
set(CPACK_BINARY_DRAGNDROP ON)
set(CPACK_PACKAGE_VERSION_MAJOR "${TROLLEDIT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TROLLEDIT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TROLLEDIT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${TROLLEDIT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${TROLLEDIT_VENDOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${TROLLEDIT_README}")
