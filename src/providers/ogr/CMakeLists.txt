
SET (OGR_SRCS qgsogrprovider.cpp)

INCLUDE_DIRECTORIES(
  .
  ../../core
  ${GDAL_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
)

ADD_LIBRARY(ogrprovider SHARED ${OGR_SRCS})

TARGET_LINK_LIBRARIES(ogrprovider
  ${QT_LIBRARY}
  ${GDAL_LIBRARY}
  ${GEOS_LIBRARY}
  qgis_core
)

INSTALL (TARGETS ogrprovider
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION lib) 