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

XERCES_CFLAGS = -I/home/morb/xerces/xerces-c-src_2_7_0/src/

if !HAVE_QTMAC
PKGDATAPATH=-DPKGDATAPATH=\"$(pkgdatadir)\"
endif

lib_LTLIBRARIES = libqgis_xsd.la

%.moc.cpp: %.h
	$(MOC) -o $@ $<

qgis_MOC = 	qgsxsddommodel.moc.cpp		\
		qgsxsdxerceswrapper.moc.cpp


libqgis_xsd_la_SOURCES =	qgsxsddommodel.cpp 		\
				qgsxsddommodel.moc.cpp		\
				qgsxsdxerceswrapper.cpp		\
				qgsxsdxerceswrapper.moc.cpp


libqgis_xsd_la_LIBFLAGS = $(QT_LDADD) $(XERCES_LDADD)

libqgis_xsd_la_LDFLAGS = -version-info $(INTERFACE_VERSION)
libqgis_xsd_la_CXXFLAGS = $(PKGDATAPATH) -I../gui/ -I../ui/ -I../core/ \
				$(CXXFLAGS) $(EXTRA_CXXFLAGS) \
				$(GDAL_CFLAGS) $(QT_CXXFLAGS) $(DEBUG_QGIS) $(GEOS_CFLAGS) \
				$(XERCES_CFLAGS)


BUILT_SOURCES = $(qgis_MOC)

CLEANFILES = $(BUILT_SOURCES)
