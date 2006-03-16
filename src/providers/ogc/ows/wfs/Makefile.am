INCLUDES = -I../../../../gui -I../../../../core

%.moc.cpp: %.h
	$(MOC) -o $@ $<

plugindir = ${pkglibdir}
plugin_LTLIBRARIES     = wfsprovider.la


## non-uic files generated from MOC

wfsprovider_MOC =	qgswfsprovider.moc.cpp

wfsprovider_la_SOURCES =					\
		qgswfsprovider.cpp				\
		qgswfsprovider.moc.cpp				\
		qgswfsprovider.h

BUILT_SOURCES = $(wfsprovider_MOC)

wfsprovider_la_LIBADD = $(QT_LDADD)
wfsprovider_la_LDFLAGS = -avoid-version -module
wfsprovider_la_CXXFLAGS = $(EXTRA_CXXFLAGS) $(QT_CXXFLAGS) $(DEBUG_QGIS) 

CLEANFILES = $(BUILT_SOURCES)
