
SET (WMS_SRCS      qgswmsprovider.cpp qgshttptransaction.cpp)
SET (WMS_MOC_HDRS  qgswmsprovider.h   qgshttptransaction.h)

QT4_WRAP_CPP (WMS_MOC_SRCS ${WMS_MOC_HDRS})

INCLUDE_DIRECTORIES( . ../../core)

ADD_LIBRARY(wmsprovider SHARED ${WMS_SRCS} ${WMS_MOC_HDRS})

TARGET_LINK_LIBRARIES(wmsprovider
  ${QT_LIBRARY}
  qgis_core
)

INSTALL (TARGETS wmsprovider
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR}) 
