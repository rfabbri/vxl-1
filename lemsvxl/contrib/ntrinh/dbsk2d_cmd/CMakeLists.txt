# contrib/ntrinh/cmd/CMakeLists.txt
# Command line programs for ntrinh



include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )


set(dbsk2d_cmd_sources
   dbsk2d_cmd.cxx
   dbsk2d_cmd_utils.h    dbsk2d_cmd_utils.cxx
)

add_executable(dbsk2d_cmd ${dbsk2d_cmd_sources})

target_link_libraries(dbsk2d_cmd dbsk2d_algo)




