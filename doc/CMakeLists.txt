
SET (DOC_FILES
favicon.ico
index.html
install.pdf
qgisdoc.css
README
userguide.pdf
images/qgis_new_80pct.png)

#SUBDIRS (images plugins install_guide)

INSTALL(FILES ${DOC_FILES}
        DESTINATION ${QGIS_DATA_DIR}/doc)
