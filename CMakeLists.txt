PROJECT(TrollEdit)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Build lua and lpeg libs
INCLUDE_DIRECTORIES(src/lua-5.1.4/src)
SUBDIRS(src/lua-5.1.4 src/lpeg-0.9)

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

