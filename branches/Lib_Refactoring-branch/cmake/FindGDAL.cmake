
# CMake module to search for GDAL library
#
# If it's found it sets GDAL_FOUND to TRUE
# and following variables are set:
#    GDAL_INCLUDE_DIR
#    GDAL_LIBRARY


FIND_PATH(GDAL_INCLUDE_DIR gdal.h /usr/include /usr/local/include)

FIND_LIBRARY(GDAL_LIBRARY NAMES gdal PATH /usr/lib /usr/local/lib)

IF (GDAL_INCLUDE_DIR AND GDAL_LIBRARY)
   SET(GDAL_FOUND TRUE)
ENDIF (GDAL_INCLUDE_DIR AND GDAL_LIBRARY)


IF (GDAL_FOUND)

   IF (NOT GDAL_FIND_QUIETLY)
      MESSAGE(STATUS "Found GDAL: ${GDAL_LIBRARY}")
   ENDIF (NOT GDAL_FIND_QUIETLY)

ELSE (GDAL_FOUND)

   IF (GDAL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GDAL")
   ENDIF (GDAL_FIND_REQUIRED)

ENDIF (GDAL_FOUND)
