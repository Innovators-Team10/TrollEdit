# Copyright (C) 2012 TrollEdit.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with TrollEdit.

project ( TrollEdit CXX C )
cmake_minimum_required ( VERSION 2.8 )

# --------------
# TrollEdit Info
# --------------

set ( TROLLEDIT_NAME "TrollEdit" )
set ( TROLLEDIT_LONGNAME "TrollEdit - Graphically Enhanced Text Editor" )
set ( TROLLEDIT_VERSION_MAJOR "1" )
set ( TROLLEDIT_VERSION_MINOR "3" )
set ( TROLLEDIT_VERSION_PATCH "4" )
set ( TROLLEDIT_VERSION_BUILD "0" )
set ( TROLLEDIT_VERSION "${TROLLEDIT_VERSION_MAJOR}.${TROLLEDIT_VERSION_MINOR}.${TROLLEDIT_VERSION_PATCH}" )
set ( TROLLEDIT_VENDOR "TrollEdit Dev Team" )
set ( TROLLEDIT_COPYRIGHT_YEAR "2012" )
set ( TROLLEDIT_DOMAIN_FIRST "trolledit" )
set ( TROLLEDIT_DOMAIN_SECOND "org" )
set ( TROLLEDIT_DOMAIN "${TROLLEDIT_DOMAIN_FIRST}.${TROLLEDIT_DOMAIN_SECOND}" )

# --------------
# Build Settings
# --------------

# Install destinations
set ( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set ( INSTALL_DATA share/trolledit CACHE PATH "Directory for shared data." )
set ( INSTALL_DEPS . )
set ( INSTALL_PLUGIN bin)
set ( INSTALL_QTCONF bin)

# Warnings for Debug mode
#if ( CMAKE_COMPILER_IS_GNUCXX )
#  set ( CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror" )
#endif ()

# Default build type
if ( NOT CMAKE_BUILD_TYPE )
  set ( CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "" FORCE )
endif ()

# Default install prefix
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  set ( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/TrollEdit CACHE PATH "Installation Destination" FORCE )
endif ()

# Build lua and lpeg libs if needed
option ( USE_BUILTIN_LUA "Use builtin LuaJIT2 and lpeg" ON )

# ------------
# Dependencies
# ------------

# Build dependencies
if ( USE_BUILTIN_LUA )
  # LuaJIT2 specific build settings on Apple
  if ( APPLE )
    set ( CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000" )
  endif ()

  # Set search paths to the dependencies
  set ( DEP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies )
  set ( DEP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dependencies )
  set ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEP_BIN}/lib ${DEP_BIN}/bin )
  set ( CMAKE_INCLUDE_PATH ${CMaKE_INCLUDE_PATH} ${DEP_BIN}/include )

  # Build LuaJIT2
  include ( ExternalProject )
  ExternalProject_Add ( dep_luajit
      PREFIX luajit
      SOURCE_DIR ${DEP_SOURCE}/luajit
      BINARY_DIR luajit
      CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    )
    
  # Build lpeg
  ExternalProject_Add ( dep_lpeg
      PREFIX lpeg
      SOURCE_DIR ${DEP_SOURCE}/lpeg
      BINARY_DIR lpeg
      CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    )
    
  # Ensure correct order of build
  add_dependencies ( dep_lpeg dep_luajit )
  
  # Define LUA_ variables
  set ( LUA_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
  if ( WIN32 )
    set ( LUA_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
  endif ()
  set ( LUA_INCLUDE_DIR ${DEP_BIN}/include )
else ()
  # Find Lua implementation, assuming lpeg is installed by other means
  find_package ( Lua51 REQUIRED )
endif ()

# Find QT4
find_package ( Qt4 COMPONENTS QtMain QtCore QtGui QtWebKit REQUIRED )
include ( ${QT_USE_FILE} )

# -----------------
# Platform Settings
# -----------------

# Apple specific overrides, we build and app bundle
if ( APPLE )
  # Executable settings
  set ( TROLLEDIT_EXECUTABLE_TYPE MACOSX_BUNDLE )
  set ( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${TROLLEDIT_NAME}.app" )

  # Override default install destinations into the bundle
  set ( INSTALL_BUNDLE . )
  set ( INSTALL_BIN     ${TROLLEDIT_NAME}.app/Contents/MacOS )
  set ( INSTALL_DATA    ${TROLLEDIT_NAME}.app/Contents/share/trolledit )
  set ( INSTALL_RES     ${TROLLEDIT_NAME}.app/Contents/Resources )
  set ( INSTALL_DEPS    ${TROLLEDIT_NAME}.app/Contents )
  set ( INSTALL_PLUGIN  ${TROLLEDIT_NAME}.app/Contents/MacOS)
  set ( INSTALL_QTCONF  ${TROLLEDIT_NAME}.app/Contents/Resources)

  # Bundle settings
  set ( MACOSX_BUNDLE_INFO_STRING "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}" )
  set ( MACOSX_BUNDLE_BUNDLE_VERSION "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}" )
  set ( MACOSX_BUNDLE_LONG_VERSION_STRING "${TROLLEDIT_NAME} ${TROLLEDIT_VERSION}" )
  set ( MACOSX_BUNDLE_SHORT_VERSION_STRING "${TROLLEDIT_VERSION}" )
  set ( MACOSX_BUNDLE_COPYRIGHT "${TROLLEDIT_COPYRIGHT_YEAR} ${TROLLEDIT_VENDOR}" )
  set ( MACOSX_BUNDLE_ICON_FILE "trolledit_app.icns" )
  set ( MACOSX_BUNDLE_GUI_IDENTIFIER "${TROLLEDIT_DOMAIN_SECOND}.${TROLLEDIT_DOMAIN_FIRST}.${TROLLEDIT_NAME}" )
  set ( MACOSX_BUNDLE_BUNDLE_NAME "${TROLLEDIT_NAME}" )

  # CPack Settings
  set ( CPACK_GENERATOR "DragNDrop" )
  set ( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/trolledit_pkg.icns" )
  set ( CPACK_PACKAGE_FILE_NAME "${TROLLEDIT_NAME}-${TROLLEDIT_VERSION}" )

  # OSX Specific resurces
  install ( FILES res/trolledit_app.icns DESTINATION ${INSTALL_RES} )
endif ()

# Windows specific overrides
if ( WIN32 )
  # Executable settings
  set ( TROLLEDIT_EXECUTABLE_TYPE WIN32 )
  set ( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/${TROLLEDIT_NAME}.exe" )
  
  # CPack settings
  set ( CPACK_GENERATOR "NSIS" )  
  set ( CPACK_PACKAGE_INSTALL_DIRECTORY "${TROLLEDIT_NAME}" )
  set ( CPACK_PACKAGE_FILE_NAME "${TROLLEDIT_NAME}-${TROLLEDIT_VERSION}" )
  set ( CPACK_PACKAGE_EXECUTABLES "${TROLLEDIT_NAME}" "${TROLLEDIT_NAME}" )
  set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}\\\\LICENSE.txt" )

  # NSIS branding
  set ( CPACK_NSIS_INSTALLED_ICON_NAME "${INSTALL_BIN}\\\\${TROLLEDIT_NAME}${CMAKE_EXECUTABLE_SUFFIX}" )
  set ( CPACK_NSIS_DISPLAY_NAME "${TROLLEDIT_NAME}" )
  set ( CPACK_NSIS_HELP_LINK "http:\\\\\\\\${TROLLEDIT_DOMAIN}" )
  set ( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\${TROLLEDIT_DOMAIN}" )
  set ( CPACK_NSIS_CONTACT "http:\\\\\\\\${TROLLEDIT_DOMAIN}" )
  set ( CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\trolledit_pkg.ico" )
  set ( CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\trolledit_rmv.ico" )
  set ( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\logo2.bmp" )
endif ()

# ---------------
# Build TrollEdit
# ---------------

# Generate trolledit.h
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/src/trolledit.h.in 
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/trolledit.h
                 @ONLY )

# Include Paths
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LUA_INCLUDE_DIR} )

# Files
file ( GLOB_RECURSE TROLLEDIT_SRC src/*.cpp )
file ( GLOB_RECURSE TROLLEDIT_H src/*.h )
file ( GLOB_RECURSE TROLLEDIT_UI ui/*.ui )
file ( GLOB_RECURSE TROLLEDIT_QRC res/*.qrc )
file ( GLOB_RECURSE TROLLEDIT_RC res/*.rc )
set ( TROLLEDIT_LICENSE "LICENSE.txt" )
set ( TROLLEDIT_README "README.md" )

# Qt Generators
qt4_wrap_ui ( TROLLEDIT_UI_GEN ${TROLLEDIT_UI} )
qt4_add_resources ( TROLLEDIT_QRC_GEN ${TROLLEDIT_QRC} )
qt4_wrap_cpp ( TROLLEDIT_MOC ${TROLLEDIT_H} )

# Build and link
add_executable ( ${TROLLEDIT_NAME} ${TROLLEDIT_EXECUTABLE_TYPE} 
  ${TROLLEDIT_SRC}
  ${TROLLEDIT_UI_GEN}
  ${TROLLEDIT_QRC_GEN}
  ${TROLLEDIT_RC}
  ${TROLLEDIT_MOC} )
  
target_link_libraries ( ${TROLLEDIT_NAME} 
  ${QT_LIBRARIES} 
  ${LUA_LIBRARY} )

# -------
# Install
# -------

# Trolledit and data
install ( TARGETS ${TROLLEDIT_NAME} 
  BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
  RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )
  
install ( FILES ${TROLLEDIT_LICENSE} ${TROLLEDIT_README} 
  DESTINATION ${INSTALL_DATA} COMPONENT Data)
install ( DIRECTORY data/ 
  DESTINATION ${INSTALL_DATA} COMPONENT Data)

# Install built in Lua unless we are building an OSX Bundle
if ( USE_BUILTIN_LUA )
  # Make sure dependencies are built
  add_dependencies ( ${TROLLEDIT_NAME} dep_luajit )

  # Install all dependencies
  install ( DIRECTORY ${DEP_BIN}/
    DESTINATION ${INSTALL_DEPS}
    COMPONENT Dependencies
    USE_SOURCE_PERMISSIONS
  )
endif ()

# Include QT4 libraries in Apple bundle and on Windows
if ( WIN32 OR APPLE )
  # On Apple plugins need to be bundled
  if ( APPLE )
    # Install Qt Plugins
    install ( DIRECTORY "${QT_PLUGINS_DIR}/imageformats" 
      DESTINATION ${INSTALL_PLUGIN}/plugins
      COMPONENT Runtime )
  
    # Install Qt Config
    install ( CODE "
      file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF}/qt.conf\" \"\")
      " COMPONENT Runtime)
  endif ()

  # Search Dirs
  string ( REPLACE "/lib" "/bin" QT_RUNTIME_DIR ${QT_LIBRARY_DIR} )
  set ( BUNDLE_DIRS ${QT_LIBRARY_DIR} ${QT_RUNTIME_DIR} ${DEP_BIN}/bin ${DEP_BIN}/lib )

  # Bundle libraries
  install ( CODE "
    file ( GLOB_RECURSE LUA_PLUGINS
        \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_DEPS}/lib/lua/*${CMAKE_SHARED_MODULE_SUFFIX}\" )
    file ( GLOB_RECURSE BUNDLE_PLUGINS
        \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
    include ( BundleUtilities )
    fixup_bundle ( \"${BUNDLE_APP}\" \"\${BUNDLE_PLUGINS};\${LUA_PLUGINS}\" \"${BUNDLE_DIRS}\" )
    " COMPONENT Runtime )
endif ()

# -------
# Packing
# -------

set ( CPACK_MONOLITHIC_INSTALL 1 )
set ( CPACK_STRIP_FILES ON )
set ( CPACK_BINARY_DRAGNDROP ON )
set ( CPACK_PACKAGE_VERSION_MAJOR "${TROLLEDIT_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR "${TROLLEDIT_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH "${TROLLEDIT_VERSION_PATCH}" )
set ( CPACK_PACKAGE_VERSION "${TROLLEDIT_VERSION}" )
set ( CPACK_PACKAGE_VENDOR "${TROLLEDIT_VENDOR}" )
set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${TROLLEDIT_README}" )
#set ( CPACK_COMPONENTS_ALL Runtime Dependencies Data )
include ( CPack )
