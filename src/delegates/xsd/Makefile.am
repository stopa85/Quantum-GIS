# Copyright (C) 2006 by Brendan Morley
# morb at beagle dot com dot au
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# $Id:$

if !HAVE_QTMAC
PKGDATAPATH=-DPKGDATAPATH=\"$(pkgdatadir)\"
endif

lib_LTLIBRARIES = libqgis_xsddelegate.la

%.moc.cpp: %.h
	$(MOC) -o $@ $<

qgis_MOC = 	qgsxsddelegate.moc.cpp


libqgis_xsddelegate_la_SOURCES =	qgsxsddelegate.cpp 		\
					qgsxsddelegate.moc.cpp


libqgis_xsddelegate_la_LIBFLAGS = $(QT_LDADD)

libqgis_xsddelegate_la_LDFLAGS = -version-info $(INTERFACE_VERSION)
libqgis_xsddelegate_la_CXXFLAGS = $(PKGDATAPATH) -I../gui/ -I../ui/ -I../core/ \
				$(CXXFLAGS) $(EXTRA_CXXFLAGS) \
				$(GDAL_CFLAGS) $(QT_CXXFLAGS) $(DEBUG_QGIS) $(GEOS_CFLAGS)


BUILT_SOURCES = $(qgis_MOC)

CLEANFILES = $(BUILT_SOURCES)
