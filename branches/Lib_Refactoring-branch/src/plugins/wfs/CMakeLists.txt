
########################################################
# Files

SET (WFS_SRCS
     qgswfsplugin.cpp
     qgswfssourceselect.cpp
     ../../app/qgsnewhttpconnection.cpp
)

SET (WFS_UIS
     qgswfssourceselectbase.ui
     ../../ui/qgslayerprojectionselectorbase.ui
     ../../ui/qgsprojectionselectorbase.ui
     ../../ui/qgsnewhttpconnectionbase.ui
)

SET (WFS_MOC_HDRS
     qgswfsplugin.h
     qgswfssourceselect.h
     ../../app/qgsnewhttpconnection.h
)

#SET (WFS_RCCS  northarrow_plugin.qrc)

########################################################
# Build

QT4_WRAP_UI (WFS_UIS_H  ${WFS_UIS})

QT4_WRAP_CPP (WFS_MOC_SRCS  ${WFS_MOC_HDRS} ${WFS_UIS_H})

#QT4_ADD_RESOURCES(WFS_RCC_SRCS ${WFS_RCCS})

ADD_LIBRARY (wfsplugin SHARED ${WFS_SRCS} ${WFS_MOC_SRCS}) # ${WFS_RCC_SRCS})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_BINARY_DIR}
     ../../core ../../core/raster ../../core/renderer ../../core/symbology
     ../../gui
     ..
     ../../providers/wfs
     ../../app # for QgsNewHttpConnection
)

TARGET_LINK_LIBRARIES(wfsplugin
  ${QT_LIBRARIES}
  qgis_core
  qgis_gui
  wfsprovider
)


########################################################
# Install

INSTALL(TARGETS wfsplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})
