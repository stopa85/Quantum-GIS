

ADD_CUSTOM_TARGET (python ALL DEPENDS run_sip compile_python_core compile_python_gui)

# Step 1: during configuration
# create file configure.py from configure.py.in
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/configure.py.in
               ${CMAKE_CURRENT_SOURCE_DIR}/configure.py)

# Step 2: during make
# run python configure.py
ADD_CUSTOM_COMMAND(OUTPUT run_sip PRE_BUILD
                   COMMAND ${PYTHON_EXECUTABLE}
                   ARGS ${CMAKE_CURRENT_SOURCE_DIR}/configure.py)

# Step 3: run make in core and gui subdirs
ADD_CUSTOM_COMMAND(OUTPUT compile_python_core PRE_LINK
                   COMMAND ${CMAKE_MAKE_PROGRAM}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/core)
ADD_CUSTOM_COMMAND(OUTPUT compile_python_gui PRE_LINK
                   COMMAND ${CMAKE_MAKE_PROGRAM}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gui)


# python's site-packages dir: bindings will be installed here
SET (CMD "
import sys
v = sys.version_info
print sys.exec_prefix + '/lib/python' + str(v[0]) + '.' + str(v[1]) + '/site-packages'
")
EXEC_PROGRAM(${PYTHON_EXECUTABLE} ARGS -c "\"${CMD}\"" OUTPUT_VARIABLE SITE_PKG_PATH)


# Step 4: install built libs to python's site packages
SET(BINDINGS_LIBS ${CMAKE_CURRENT_BINARY_DIR}/core/core.so
                  ${CMAKE_CURRENT_BINARY_DIR}/gui/gui.so)

INSTALL(FILES __init__.py qgisconfig.py ${BINDINGS_LIBS} DESTINATION ${SITE_PKG_PATH}/qgis)