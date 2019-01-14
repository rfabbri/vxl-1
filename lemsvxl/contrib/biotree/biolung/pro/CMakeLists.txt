# This is contrib/biotree/biolung/pro/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(biolung_process_sources
   biolung_process.cxx          biolung_process.h
)

add_library(biolung_process ${biolung_process_sources})

target_link_libraries(biolung_process biolung vidpro1)

if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)
