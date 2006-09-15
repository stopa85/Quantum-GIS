
# TODO: delete qgsvectorsymbologywidget.cpp and qgsvectorsymbologywidget.h from SVN

SET(QGIS_APP_SRCS
main.cpp
qgisapp.cpp
qgisappinterface.cpp
qgsabout.cpp
qgsaddattrdialog.cpp
qgsattributeactiondialog.cpp
qgsattributedialog.cpp
qgsattributetable.cpp
qgsattributetabledisplay.cpp
qgsbookmarkitem.cpp
qgsbookmarks.cpp
qgsclipboard.cpp
qgscontinuouscolordialog.cpp
qgscustomprojectiondialog.cpp
qgsdelattrdialog.cpp
qgsencodingfiledialog.cpp
qgsfillstylewidget.cpp
qgsgeomtypedialog.cpp
qgsgraduatedsymboldialog.cpp
qgshelpviewer.cpp
qgsidentifyresults.cpp
qgslabeldialog.cpp
qgslayerprojectionselector.cpp
qgslinestyledialog.cpp
qgslinestylewidget.cpp
qgsludialog.cpp
qgsmapserverexport.cpp
qgsmaptoolcapture.cpp
qgsmaptoolidentify.cpp
qgsmaptoolselect.cpp
qgsmaptoolvertexedit.cpp
qgsmarkerdialog.cpp
qgsmeasure.cpp
qgsnewhttpconnection.cpp
qgsnumericsortlistviewitem.cpp
qgsoptions.cpp
qgspastetransformations.cpp
qgspatterndialog.cpp
qgspluginitem.cpp
qgspluginmanager.cpp
qgspluginmetadata.cpp
qgspluginregistry.cpp
qgspointstylewidget.cpp
qgsprojectproperties.cpp
qgsrasterlayerproperties.cpp
qgssearchquerybuilder.cpp
qgsserversourceselect.cpp
qgssinglesymboldialog.cpp
qgsuniquevaluedialog.cpp
qgsvectorlayerproperties.cpp

composer/qgscomposer.cpp
composer/qgscomposeritem.cpp
composer/qgscomposerlabel.cpp
composer/qgscomposerpicture.cpp
composer/qgscomposermap.cpp
composer/qgscomposerscalebar.cpp
composer/qgscomposervectorlegend.cpp
composer/qgscomposerview.cpp
composer/qgscomposition.cpp

legend/qgslegendgroup.cpp
legend/qgslegend.cpp
legend/qgslegenditem.cpp
legend/qgslegendlayer.cpp
legend/qgslegendlayerfile.cpp
legend/qgslegendlayerfilegroup.cpp
legend/qgslegendpropertygroup.cpp
legend/qgslegendpropertyitem.cpp
legend/qgslegendsymbologygroup.cpp
legend/qgslegendsymbologyitem.cpp
legend/qgslegendvectorsymbologyitem.cpp
)

SET(QGIS_APP_UIS
../ui/qgisappbase.ui
../ui/qgsabout.ui
../ui/qgsaddattrdialogbase.ui
../ui/qgsattributeactiondialogbase.ui
../ui/qgsattributedialogbase.ui
../ui/qgsattributetablebase.ui
../ui/qgsbookmarksbase.ui
../ui/qgscomposerbase.ui
../ui/qgscomposerlabelbase.ui
../ui/qgscomposermapbase.ui
../ui/qgscomposerpicturebase.ui
../ui/qgscomposerscalebarbase.ui
../ui/qgscomposervectorlegendbase.ui
../ui/qgscompositionbase.ui
../ui/qgscontinuouscolordialogbase.ui
../ui/qgscustomprojectiondialogbase.ui
../ui/qgsdbsourceselectbase.ui
../ui/qgsdelattrdialogbase.ui
../ui/qgsfillstylewidgetbase.ui
../ui/qgsgeomtypedialogbase.ui
../ui/qgsgraduatedsymboldialogbase.ui
../ui/qgshelpviewerbase.ui
../ui/qgsidentifyresultsbase.ui
../ui/qgslabeldialogbase.ui
../ui/qgslayerprojectionselectorbase.ui
../ui/qgslinestyledialogbase.ui
../ui/qgslinestylewidgetbase.ui
../ui/qgsludialogbase.ui
../ui/qgsmapserverexportbase.ui
../ui/qgsmarkerdialogbase.ui
../ui/qgsmeasurebase.ui
../ui/qgsmessageviewer.ui
../ui/qgsnewconnectionbase.ui
../ui/qgsnewhttpconnectionbase.ui
../ui/qgsoptionsbase.ui
../ui/qgspastetransformationsbase.ui
../ui/qgspatterndialogbase.ui
../ui/qgspgquerybuilderbase.ui
../ui/qgspluginmanagerbase.ui
../ui/qgspointstylewidgetbase.ui
../ui/qgsprojectionselectorbase.ui
../ui/qgsprojectpropertiesbase.ui
../ui/qgsrasterlayerpropertiesbase.ui
../ui/qgsserversourceselectbase.ui
../ui/qgssinglesymboldialogbase.ui
../ui/qgsuniquevaluedialogbase.ui
../ui/qgsvectorlayerpropertiesbase.ui
../ui/qgsvectorsymbologywidgetbase.ui
)

QT4_WRAP_UI(QGIS_APP_UIS_H ${QGIS_APP_UIS})

SET (QGIS_APP_MOC_HDRS

qgisapp.h
qgisappinterface.h
qgsabout.h
qgsaddattrdialog.h
qgsattributeactiondialog.h
qgsattributedialog.h
qgsattributetabledisplay.h
qgsattributetable.h
qgsbookmarks.h
qgscontinuouscolordialog.h
qgscustomprojectiondialog.h
qgsdelattrdialog.h
qgsencodingfiledialog.h
qgsfillstylewidget.h
qgsgeomtypedialog.h
qgsgraduatedsymboldialog.h
qgshelpviewer.h
qgsidentifyresults.h
qgslabeldialog.h
qgslayerprojectionselector.h
qgslinestyledialog.h
qgslinestylewidget.h
qgsludialog.h
qgsmaplayerinterface.h
qgsmapserverexport.h
qgsmarkerdialog.h
qgsmeasure.h
qgsnewhttpconnection.h
qgsoptions.h
qgspastetransformations.h
qgspatterndialog.h
qgspluginmanager.h
qgspointstylewidget.h
qgsprojectproperties.h
qgsrasterlayerproperties.h
qgssearchquerybuilder.h
qgsserversourceselect.h
qgssinglesymboldialog.h
qgsuniquevaluedialog.h
qgsvectorlayerproperties.h

composer/qgscomposer.h
composer/qgscomposerlabel.h
composer/qgscomposerpicture.h
composer/qgscomposermap.h
composer/qgscomposerscalebar.h
composer/qgscomposervectorlegend.h
composer/qgscomposerview.h
composer/qgscomposition.h

legend/qgslegend.h
legend/qgslegendlayerfile.h
)

IF (POSTGRES_FOUND)
  SET (QGIS_APP_SRCS ${QGIS_APP_SRCS}
          qgsdbsourceselect.cpp
		  qgsnewconnection.cpp
		  qgspgquerybuilder.cpp
  )
  SET (QGIS_APP_MOC_HDRS ${QGIS_APP_MOC_HDRS}
          qgsdbsourceselect.h
		  qgsnewconnection.h
		  qgspgquerybuilder.h
  )

  ADD_DEFINITIONS(-DHAVE_POSTGRESQL)

ENDIF (POSTGRES_FOUND)

IF (WIN32)

  # Icon for windows
  # Note: can't include .rc directly to source files
  # as it's ignored when used MinGW

  IF (NOT WINDRES)
    FIND_PROGRAM(WINDRES windres)
    IF (NOT WINDRES)
      MESSAGE(FATAL_ERROR "windres not found - aborting")
    ENDIF (NOT WINDRES)
  ENDIF (NOT WINDRES)

  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qgis_win32.obj
                     COMMAND ${WINDRES}  -I${CMAKE_CURRENT_SOURCE_DIR}/..
                                         -i${CMAKE_CURRENT_SOURCE_DIR}/../qgis_win32.rc
                                         -o ${CMAKE_CURRENT_BINARY_DIR}/qgis_win32.obj)
  SET(QGIS_APP_SRCS ${QGIS_APP_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/qgis_win32.obj)

ENDIF (WIN32)

# This is the right way to include icon in MSVC but currently
# it ends with error that qgis_win322.ico is old DIB and it has to be converted
#IF (MSVC)
#  SET (QGIS_APP_SRCS ${QGIS_APP_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../qgis_win32.rc)
#ENDIF (MSVC)


# TODO: better way for ui?
QT4_WRAP_CPP(QGIS_APP_MOC_SRCS ${QGIS_APP_MOC_HDRS} ${QGIS_APP_UIS_H})

INCLUDE_DIRECTORIES(.
     ./composer ./legend
     ${CMAKE_CURRENT_BINARY_DIR}
     ../.. # for qgsconfig.h
     ../core
     ../core/raster ../core/renderer ../core/symbology
     ../gui
     ../plugins
  ${PROJ_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
)

IF (POSTGRES_FOUND)
  INCLUDE_DIRECTORIES(${POSTGRES_INCLUDE_DIR})
ENDIF (POSTGRES_FOUND)


#############

ADD_EXECUTABLE(qgis ${QGIS_APP_UIS_H} ${QGIS_APP_MOC_SRCS} ${QGIS_APP_SRCS})

TARGET_LINK_LIBRARIES(qgis
  ${QT_LIBRARIES}
  qgis_core
  qgis_gui
)

IF (POSTGRES_FOUND)
  TARGET_LINK_LIBRARIES (qgis ${POSTGRES_LIBRARY})
ENDIF (POSTGRES_FOUND)

INSTALL(TARGETS qgis  RUNTIME DESTINATION ${QGIS_BIN_DIR})
