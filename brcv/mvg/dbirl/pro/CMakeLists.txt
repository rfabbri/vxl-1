# This is brl/blem/brcv/mvg/dbirl/pro/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories(${VXL_GEL_INCLUDE_DIR} )

set(dbirl_pro_sources
    dbirl_h_computation_process.cxx       dbirl_h_computation_process.h
)


add_library(dbirl_pro ${dbirl_pro_sources})

target_link_libraries(dbirl_pro dbirl dbinfo vidpro1)


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)

