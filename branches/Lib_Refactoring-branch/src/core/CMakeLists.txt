
###ADD_DEFINITIONS(-DBUILD_QGIS_CORE)

#############################################################
# sources

SET(QGIS_CORE_SRCS
qgis.cpp
qgsapplication.cpp
qgsattributeaction.cpp
qgsclipper.cpp
qgscontexthelp.cpp
qgscoordinatetransform.cpp
qgsdistancearea.cpp
qgsexception.cpp
qgsfeature.cpp
qgsfeatureattribute.cpp
qgsfield.cpp
qgsgeometry.cpp
qgsgeometryvertexindex.cpp
qgslabel.cpp
qgslabelattributes.cpp
qgsline.cpp
qgslogger.cpp
qgsmaplayer.cpp
qgsmaplayerregistry.cpp
qgsmaprender.cpp
qgsmaptopixel.cpp
qgsmessageoutput.cpp
qgspoint.cpp
qgsproject.cpp
qgsprojectproperty.cpp
qgsprovidercountcalcevent.cpp
qgsproviderextentcalcevent.cpp
qgsprovidermetadata.cpp
qgsproviderregistry.cpp
qgsrasterdataprovider.cpp
qgsrect.cpp
qgsrunprocess.cpp
qgsscalecalculator.cpp
qgssearchstring.cpp
qgssearchtreenode.cpp
qgsspatialrefsys.cpp
qgsvectordataprovider.cpp
qgsvectorfilewriter.cpp
qgsvectorlayer.cpp

raster/qgscolortable.cpp
raster/qgsrasterlayer.cpp

renderer/qgscontinuouscolorrenderer.cpp
renderer/qgsgraduatedsymbolrenderer.cpp
renderer/qgsrangerenderitem.cpp
renderer/qgsrenderer.cpp
renderer/qgsrenderitem.cpp
renderer/qgssinglesymbolrenderer.cpp
renderer/qgsuniquevaluerenderer.cpp

symbology/qgslinesymbol.cpp
symbology/qgsmarkercatalogue.cpp
symbology/qgsmarkersymbol.cpp
symbology/qgspolygonsymbol.cpp
symbology/qgssymbol.cpp
symbology/qgssymbologyutils.cpp
)

ADD_FLEX_FILES(QGIS_CORE_SRCS qgssearchstringlexer.ll)

ADD_BISON_FILES(QGIS_CORE_SRCS qgssearchstringparser.yy)

SET(QGIS_CORE_MOC_HDRS
qgscontexthelp.h
qgscoordinatetransform.h
qgsdataprovider.h
qgsmaplayer.h
qgsmaplayerregistry.h
qgsmaprender.h
qgsmessageoutput.h
qgsproject.h
qgsrasterdataprovider.h
qgsrunprocess.h
qgsvectorlayer.h

raster/qgsrasterlayer.h
)

QT4_WRAP_CPP(QGIS_CORE_MOC_SRCS ${QGIS_CORE_MOC_HDRS})

# TODO: ../.. because of qgsconfig.h
INCLUDE_DIRECTORIES( ../.. raster renderer symbology .
  ${QT_QTSVG_INCLUDE_DIR}
  ${PROJ_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
)

#############################################################
# qgis_core library

ADD_LIBRARY(qgis_core SHARED ${QGIS_CORE_SRCS} ${QGIS_CORE_MOC_SRCS})

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(qgis_core
  ${QT_LIBRARIES} ${QT_QTSVG_LIBRARY}
  ${PROJ_LIBRARY}
  ${SQLITE3_LIBRARY}
  ${GEOS_LIBRARY}
  ${GDAL_LIBRARY}
  ${PLATFORM_LIBRARIES}
)

ADD_LIBRARY(qgis_core_s STATIC ${QGIS_CORE_SRCS} ${QGIS_CORE_MOC_SRCS})

TARGET_LINK_LIBRARIES(qgis_core_s
  ${QT_LIBRARIES} ${QT_QTSVG_LIBRARY}
  ${PROJ_LIBRARY}
  ${SQLITE3_LIBRARY}
  ${GEOS_LIBRARY}
  ${GDAL_LIBRARY}
  ${PLATFORM_LIBRARIES}
)


INSTALL(TARGETS qgis_core
  RUNTIME DESTINATION ${QGIS_BIN_DIR}
  LIBRARY DESTINATION lib)
