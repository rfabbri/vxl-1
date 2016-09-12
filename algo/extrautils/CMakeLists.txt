# extrautils library
# algo/extrautils/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )

set(extra_util_sources

arc.h
common.h
defines.h
FuzzyBoolean.h
geometry_functions.h
msgout.h
points.h
extrautils.cpp

)

aux_source_directory(Templates extra_util_sources)

add_library(extrautils  ${extra_util_sources})

target_link_libraries( extrautils vgl vcl vnl vbl )

