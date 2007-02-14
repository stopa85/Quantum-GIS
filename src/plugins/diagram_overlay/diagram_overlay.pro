QGIS_DIR=/home/humarco/src/qgis_diagram

TEMPLATE = lib

#app

CONFIG += qt uic4 debug

QT += xml qt3support gui network

unix:LIBS += -L$$QGIS_DIR/lib -lqgis_core -lqgis_gui

INCLUDEPATH += $$QGIS_DIR $$QGIS_DIR/src/ui $$QGIS_DIR/src/gui $$QGIS_DIR/src/core $$QGIS_DIR/src/core/renderer $$QGIS_DIR/src/core/raster $$QGIS_DIR/src/core/symbology $$QGIS_DIR/include/qgis $$QGIS_DIR/src/plugins

FORMS += qgsdiagramdialogbase.ui qgslinearlyscalingdialogbase.ui

HEADERS += qgsdiagramdialog.h qgslinearlyscalingdialog.h

SOURCES = qgsdiagramfactory.cpp qgsdiagramitem.cpp qgsdiagramrenderer.cpp qgslinearlyscalingdiagramrenderer.cpp qgsdiagramoverlay.cpp qgsdiagramoverlayplugin.cpp qgsdiagramdialog.cpp qgslinearlyscalingdialog.cpp qgsdiagramrendererwidget.cpp 

#test.cpp testwidget.cpp

DEST = diagram_overlay.so

DEFINES += GUI_EXPORT= CORE_EXPORT=