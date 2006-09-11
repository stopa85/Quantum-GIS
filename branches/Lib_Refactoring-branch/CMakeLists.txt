
PROJECT(qgis)

# BUGS:
# - linking qgis.exe on windows results in: The input line is too long
#   (need to delete moc_ui_*.obj files from build\src\app\CMakeFiles\qgis.dir\build.make)
# - using shared libraries for linking with qgis.exe (bad output exe)
# - wms (at least on linux) can't be loaded
#
# TODO:
# - includes for libs
# - python bindings
# - more providers and plugins
# - rename *.ui files to have the same filename as their implementation
#   e.g. instead of blahblahbase.ui use blahblah.ui
#   reasons:
#   1. it's more common in Qt4
#   2. build system will be simpler (no need to call QT4_WRAP_CPP for ui_*.h)
#      because dependencies are added automatically
#   (another possibility is to create custom target for building UI)

#############################################################
# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.2)

SET(CMAKE_COLOR_MAKEFILE ON)

# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})


#############################################################
# check if lexer and parser are not missing
# http://www.mail-archive.com/cmake@cmake.org/msg02861.html

INCLUDE(Flex)

FIND_FLEX()

IF (NOT FLEX_EXECUTABLE)
  MESSAGE(FATAL_ERROR "Couldn't find Flex")
ENDIF (NOT FLEX_EXECUTABLE)

INCLUDE(Bison)

FIND_BISON()

IF (NOT BISON_EXECUTABLE)
  MESSAGE(FATAL_ERROR "Couldn't find Bison")
ENDIF (NOT BISON_EXECUTABLE)

#############################################################
# search for dependencies

FIND_PACKAGE(Proj     REQUIRED)
FIND_PACKAGE(Sqlite3  REQUIRED)
FIND_PACKAGE(GEOS     REQUIRED)
FIND_PACKAGE(GDAL     REQUIRED)

#############################################################
# search for Qt4

SET( QT_USE_QT3SUPPORT TRUE )
SET( QT_USE_QTXML      TRUE )
SET( QT_USE_QTNETWORK  TRUE )
SET( QT_USE_QTSVG      TRUE )

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE( ${QT_USE_FILE} )

#############################################################
# enable warnings

ADD_DEFINITIONS( -Wall )

IF (CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DQGISDEBUG=1)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

#############################################################
# platform specific stuff

IF (WIN32)

  SET (QGIS_BIN_DIR    ${CMAKE_INSTALL_PREFIX})
  SET (QGIS_DATA_DIR   ${CMAKE_INSTALL_PREFIX})
  SET (QGIS_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/plugins)

  # without this option linking qgis_gui to qgis_core fails
  MESSAGE(STATUS "Setting -enable-runtime-pseudo-reloc")
  SET(CMAKE_SHARED_LINKER_FLAGS  ${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-runtime-pseudo-reloc)

ELSE (WIN32)

  # common for MAC and UNIX
  SET (QGIS_BIN_DIR    ${CMAKE_INSTALL_PREFIX}/bin)
  SET (QGIS_DATA_DIR   ${CMAKE_INSTALL_PREFIX}/share/qgis)
  SET (QGIS_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/qgis)

  IF (UNIX)
    ADD_DEFINITIONS(-DPREFIX=\\"${CMAKE_INSTALL_PREFIX}\\")
    ADD_DEFINITIONS(-DPLUGINPATH=\\"${QGIS_PLUGIN_DIR}\\")
    ADD_DEFINITIONS(-DPKGDATAPATH=\\"${QGIS_DATA_DIR}\\")
  ENDIF (UNIX)

ENDIF (WIN32)


#############################################################
# create qgsconfig.h

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/qgsconfig.h.in ${CMAKE_SOURCE_DIR}/qgsconfig.h)

#############################################################
# process subdirs

SUBDIRS(src doc images resources i18n)


#############################################################
# install stuff

INSTALL (FILES AUTHORS
         DESTINATION ${QGIS_DATA_DIR}/doc)
