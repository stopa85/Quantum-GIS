
SET(QGIS_GUI_UIS
  ../ui/qgsmessageviewer.ui
  ../ui/qgsprojectionselectorbase.ui
)

SET(QGIS_GUI_SRCS
qgisgui.cpp
qgisinterface.cpp
qgscursors.cpp
qgsmapcanvas.cpp
qgsmapcanvasitem.cpp
qgsmapcanvasmap.cpp
qgsmapoverviewcanvas.cpp
qgsmaptool.cpp
qgsmaptoolpan.cpp
qgsmaptoolzoom.cpp
qgsmessageviewer.cpp
qgsprojectionselector.cpp
qgsrubberband.cpp
qgsvertexmarker.cpp
)

QT4_WRAP_UI(QGIS_GUI_UIS_H ${QGIS_GUI_UIS})


SET(QGIS_GUI_MOC_HDRS
qgisinterface.h
qgsmapcanvas.h
qgsmapcanvasitem.h
qgsmapcanvasmap.h
qgsmapoverviewcanvas.h
qgsprojectionselector.h
)

# TODO: find better way to include ui_*.h in dependencies
QT4_WRAP_CPP(QGIS_GUI_MOC_SRCS ${QGIS_GUI_MOC_HDRS} ${QGIS_GUI_UIS_H})

INCLUDE_DIRECTORIES(. ../.. ../core
     ../core/raster ../core/renderer ../core/symbology
     ${SQLITE3_INCLUDE_DIR}
     ${CMAKE_CURRENT_BINARY_DIR})

#############################################################
# qgis_gui library


# ${QGIS_GUI_UIS_H}
ADD_LIBRARY(qgis_gui SHARED ${QGIS_GUI_SRCS} ${QGIS_GUI_MOC_SRCS})

TARGET_LINK_LIBRARIES(qgis_gui
  ${QT_LIBRARIES}
  ${SQLITE3_LIBRARIES}
  qgis_core
)

IF (WIN32)

ADD_LIBRARY(qgis_gui_s STATIC ${QGIS_GUI_SRCS} ${QGIS_GUI_MOC_SRCS})

TARGET_LINK_LIBRARIES(qgis_gui_s
  ${QT_LIBRARIES}
  ${SQLITE3_LIBRARIES}
  qgis_core_s
)

ENDIF (WIN32)

INSTALL(TARGETS qgis_gui
  RUNTIME DESTINATION ${QGIS_BIN_DIR}
  LIBRARY DESTINATION lib)
