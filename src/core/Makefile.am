# Copyright (C) 2003 Gary Sherman <sherman at mrcc.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

##
## ----------------------------------------------------------------------
##        Initial setup 
## ----------------------------------------------------------------------
##
if HAVE_QTMAC
MAC = mac
endif


if !HAVE_QTMAC
PREFIX=-DPREFIX=\"$(prefix)\"
PLUGINPATH=-DPLUGINPATH=\"$(pkglibdir)\"
PKGDATAPATH=-DPKGDATAPATH=\"$(pkgdatadir)\"
endif

%.moc.cpp: %.h
	$(MOC) -o $@ $<

##
## ----------------------------------------------------------------------
##      Define Targets
## ----------------------------------------------------------------------
##
lib_LTLIBRARIES = libqgis_core.la 


##
## ----------------------------------------------------------------------
##         libqgis_core stuff starts here
## ----------------------------------------------------------------------
##

libqgis_coreHEADERS =				\
		qgis.h						\
		qgsapplication.h			\
		qgsattributeaction.h		\
		qgsbookmarkitem.h			\
		qgsclipper.h				\
		qgscolortable.h				\
		qgscontexthelp.h			\
		qgscoordinatetransform.h			\
		qgscsexception.h			\
		qgscustomsymbol.h			\
		qgsdatamanager.h			\
		qgsdataprovider.h			\
		qgsdatasource.h				\
		qgsdatasourceuri.h			\
		qgsdistancearea.h			\
		qgsexception.h				\
		qgsfeature.h				\
		qgsfeatureattribute.h		\
		qgsfield.h					\
		qgsgeometry.h				\
		qgsgeometryvertexindex.h	\
		qgslabel.h					\
		qgslabelattributes.h		\
		qgsline.h					\
		qgslinesymbol.h				\
		qgslogger.h				\
		qgsmaplayer.h				\
		qgsmaplayerregistry.h		\
		qgsmaprender.h			\
		qgsmaptopixel.h				\
		qgsmarkercatalogue.h		\
		qgsmarkersymbol.h			\
		qgsmessageoutput.h		\
		qgspoint.h					\
		qgspolygonsymbol.h			\
		qgsproject.h				\
		qgsprojectproperty.h		\
		qgsprovidercountcalcevent.h	\
		qgsproviderextentcalcevent.h\
		qgsprovidermetadata.h		\
		qgsproviderregistry.h		\
		qgsrasterdataprovider.h		\
		qgsrect.h					\
		qgsrunprocess.h			\
		qgsscalecalculator.h		\
		qgssearchstring.h			\
    qgssearchtreenode.h			\
		qgsspatialrefsys.h		\
		qgssymbol.h					\
		qgssymbologyutils.h			\
		qgsvectorfilewriter.h		\
		qgsvectorlayer.h			\
		qgsvectordataprovider.h		\
    \
		raster/qgsrasterbandstats.h \
		raster/qgsrasterpyramid.h \
    raster/qgsrasterlayer.h \
	  raster/qgsrasterviewport.h \
		\
		renderer/qgscontinuouscolorrenderer.h \
		renderer/qgsgraduatedsymbolrenderer.h \
		renderer/qgsrangerenderitem.h	\
		renderer/qgsrenderer.h \
		renderer/qgsrenderitem.h \
		renderer/qgssinglesymbolrenderer.h \
		renderer/qgsuniquevaluerenderer.h
						

## files generated from MOC                             
libqgis_core_la_MOC =							\
		qgscontexthelp.moc.cpp			\
		qgscoordinatetransform.moc.cpp		\
		qgsdataprovider.moc.cpp				\
		qgsmaplayer.moc.cpp				\
		qgsmaplayerregistry.moc.cpp		\
		qgsmaprender.moc.cpp			\
		qgsmessageoutput.moc.cpp		\
		qgsproject.moc.cpp		\
		qgsrasterdataprovider.moc.cpp		\
		qgsrunprocess.moc.cpp			\
		qgsvectorlayer.moc.cpp \
		\
		raster/qgsrasterlayer.moc.cpp

libqgis_core_la_SOURCES =\
		qgis.cpp								\
		qgsapplication.cpp	\
		qgsattributeaction.cpp	\
		qgsbookmarkitem.cpp	\
		qgsclipper.cpp						\
		qgscolortable.cpp					\
		qgscontexthelp.cpp		\
		qgscoordinatetransform.cpp		\
		qgscustomsymbol.cpp		\
		qgsdatamanager.cpp				\
		qgsdatasource.cpp		\
		qgsdistancearea.cpp			\
		qgsexception.cpp				\
		qgsfeature.cpp					\
		qgsfeatureattribute.cpp				\
		qgsfield.cpp						\
		qgsgeometry.cpp						\
		qgsgeometryvertexindex.cpp			\
		qgslabel.cpp				\
		qgslabelattributes.cpp		\
		qgsline.cpp			\
		qgslinesymbol.cpp		\
		qgslogger.cpp			\
		qgsmaplayer.cpp				\
		qgsmaplayerregistry.cpp		\
		qgsmaprender.cpp			\
		qgsmaptopixel.cpp					\
		qgsmarkercatalogue.cpp				\
		qgsmarkersymbol.cpp					\
		qgsmessageoutput.cpp			\
		qgspoint.cpp						\
		qgspolygonsymbol.cpp		\
		qgsproject.cpp			\
		qgsprojectproperty.cpp			\
		qgsprovidercountcalcevent.cpp		\
		qgsproviderextentcalcevent.cpp		\
		qgsprovidermetadata.cpp		\
		qgsproviderregistry.cpp				\
		qgsrasterdataprovider.cpp	\
		qgsrect.cpp							\
		qgsrunprocess.cpp				\
		qgsscalecalculator.cpp				\
		qgssearchstring.cpp             \
		qgssearchstringlexer.ll		\
		qgssearchstringparser.yy	\
		qgssearchtreenode.cpp		\
		qgsspatialrefsys.cpp		\
		qgssymbol.cpp						\
		qgssymbologyutils.cpp		\
		qgsvectorfilewriter.cpp		\
		qgsvectorlayer.cpp		\
		qgsvectordataprovider.cpp \
		\
		raster/qgsrasterlayer.cpp \
		\
		renderer/qgscontinuouscolorrenderer.cpp \
		renderer/qgsgraduatedsymbolrenderer.cpp \
		renderer/qgssinglesymbolrenderer.cpp \
		renderer/qgsrangerenderitem.cpp \
		renderer/qgsrenderer.cpp \
		renderer/qgsrenderitem.cpp \
		renderer/qgsuniquevaluerenderer.cpp
		

AM_YFLAGS       = -d
libqgis_core_la_YACC = qgssearchstringparser.h


nodist_libqgis_core_la_SOURCES = $(libqgis_core_la_MOC)  

BUILT_SOURCES = $(libqgis_core_la_YACC) $(libqgis_core_la_MOC)  $(postgresMOC)


libqgis_core_la_LIBADD = $(QT_LDADD) $(GEOS_LDADD) $(PYTHON_LIB) $(GDAL_LDADD) -lsqlite3
libqgis_core_la_LDFLAGS = -version-info $(INTERFACE_VERSION)

libqgis_core_la_CXXFLAGS = $(PREFIX) $(PLUGINPATH) $(PKGDATAPATH) $(CXXFLAGS) $(EXTRA_CXXFLAGS) $(QT_CXXFLAGS) $(PG_INC) $(DEBUG_QGIS) $(GEOS_CFLAGS) $(PYTHON_INCLUDE_DIR) $(HAVE_PYTHON) -Iraster -Irenderer

# Link headers to include directory
all-local:
	rm -rf include/
	mkdir include
	includes='$(pkginclude_HEADERS)' ; \
	for file in $$includes ; do \
	  base=`basename $$file` ; \
		ln -s ../$$file include/$$base ; \
	done

clean-local:
	rm -rf include/


##
## ----------------------------------------------------------------------
##         additional files that need to be installed go here
## ----------------------------------------------------------------------
##
pkginclude_HEADERS = $(libqgis_coreHEADERS) 

pkginclude_dir = $(includedir)/qgis


CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST=


noinst_HEADERS = qgsdatasourceuri.h
