
########################################################
# Files

SET (ogr_SRCS
     ogr.cpp
     qgsogrsourceselect.cpp
     qgsnewogrconnection.cpp
)

SET (ogr_UIS 
     qgsogrsourceselect.ui
     qgsnewogrconnection.ui
)

SET (ogr_MOC_HDRS
     ogr.h
     qgsogrsourceselect.h
     qgsnewogrconnection.h
)

SET (ogr_RCCS  ogr.qrc)

########################################################
# Build

QT4_WRAP_UI (ogr_UIS_H  ${ogr_UIS})

QT4_WRAP_CPP (ogr_MOC_SRCS  ${ogr_MOC_HDRS})

QT4_ADD_RESOURCES(ogr_RCC_SRCS ${ogr_RCCS})

ADD_LIBRARY (ogrplugin MODULE ${ogr_SRCS} ${ogr_MOC_SRCS} ${ogr_RCC_SRCS} ${ogr_UIS_H})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_BINARY_DIR}
     ../../core ../../core/raster ../../core/renderer ../../core/symbology
     ../../gui
     ..
     ${GDAL_INCLUDE_DIR}
     ../../providers/ogr
)

TARGET_LINK_LIBRARIES(ogrplugin
  ${QT_LIBRARIES}
  qgis_core
  qgis_gui
  ${GDAL_LIBRARY}
)


########################################################
# Install

INSTALL(TARGETS ogrplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})

