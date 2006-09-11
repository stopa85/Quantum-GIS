
########################################################
# Files

SET (COPYRIGHT_SRCS
     plugin.cpp
     plugin.h
     plugingui.cpp
     plugingui.h
)

SET (COPYRIGHT_UIS pluginguibase.ui)

SET (COPYRIGHT_MOC_HDRS  plugin.h plugingui.h)

SET (COPYRIGHT_RCC_SRCS  copyright_plugin.qrc)

########################################################
# Build

QT4_WRAP_UI (COPYRIGHT_UIS_H  ${COPYRIGHT_UIS})

QT4_WRAP_CPP (COPYRIGHT_MOC_SRCS  ${COPYRIGHT_MOC_HDRS} ${COPYRIGHT_UIS_H})

QT4_ADD_RESOURCES(COPYRIGHT_RCC_SRCS ${COPYRIGHT_RCCS})

ADD_LIBRARY (copyrightlabelplugin SHARED ${COPYRIGHT_SRCS} ${COPYRIGHT_MOC_SRCS})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_BINARY_DIR}
     ../../core ../../core/raster ../../core/renderer ../../core/symbology
     ../../gui
     ..
  ${GEOS_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(copyrightlabelplugin
  ${QT_LIBRARIES}
  ${GDAL_LIBRARY}
  qgis_core
  qgis_gui
)


########################################################
# Install

INSTALL(TARGETS copyrightlabelplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})
