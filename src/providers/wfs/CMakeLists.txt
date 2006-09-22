
########################################################
# Files

SET(WFS_SRCS qgswfsprovider.cpp qgshttptransaction.cpp)

SET(WFS_MOC_HDRS qgshttptransaction.h)


########################################################
# Build

QT4_WRAP_CPP(WFS_MOC_SRCS ${WFS_MOC_HDRS})

INCLUDE_DIRECTORIES (
  ../../core
)

ADD_LIBRARY (wfsprovider SHARED ${WFS_SRCS} ${WFS_MOC_SRCS})

TARGET_LINK_LIBRARIES (wfsprovider
  ${QT_LIBRARIES}
  qgis_core
)


########################################################
# Install
                
INSTALL(TARGETS wfsprovider
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})
