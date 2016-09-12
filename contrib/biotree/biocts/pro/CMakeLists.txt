# This is contrib/biotree/biocts/pro/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/breye1 )

set(biocts_process_sources
   biocts_process.cxx          biocts_process.h
)

add_library(biocts_process ${biocts_process_sources})

target_link_libraries(biocts_process biocts vidpro1)

if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)
