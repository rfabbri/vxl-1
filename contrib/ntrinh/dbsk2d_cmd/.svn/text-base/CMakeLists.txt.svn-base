# contrib/ntrinh/cmd/CMakeLists.txt
# Command line programs for ntrinh



INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(dbsk2d_cmd_sources
   dbsk2d_cmd.cxx
   dbsk2d_cmd_utils.h    dbsk2d_cmd_utils.cxx
)

ADD_EXECUTABLE(dbsk2d_cmd ${dbsk2d_cmd_sources})

TARGET_LINK_LIBRARIES(dbsk2d_cmd dbsk2d_algo)




