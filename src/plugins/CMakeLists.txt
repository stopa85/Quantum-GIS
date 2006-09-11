
SUBDIRS (copyright_label delimited_text grid_maker navigation north_arrow scale_bar)

IF (POSTGRES_FOUND)
  SUBDIRS (geoprocessing spit)
ENDIF (POSTGRES_FOUND)

IF (EXPAT_FOUND)
  SUBDIRS (gps_importer)
ENDIF (EXPAT_FOUND)
