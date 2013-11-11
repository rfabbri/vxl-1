# extrautils library
# algo/extrautils/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(extra_util_sources

arc.h
common.h
defines.h
FuzzyBoolean.h
geometry_functions.h
msgout.h
points.h
extrautils.cpp

)

AUX_SOURCE_DIRECTORY(Templates extra_util_sources)

ADD_LIBRARY(extrautils  ${extra_util_sources})

TARGET_LINK_LIBRARIES( extrautils vgl vcl vnl vbl )

