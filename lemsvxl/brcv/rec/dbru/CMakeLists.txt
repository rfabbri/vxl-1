# brcv/rec/dbru/CMakeLists.txt
# Brown Recognition Utililities Libraries.

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/rec )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_RPL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXD_BRLD_INCLUDE_DIR})


set(dbru_sources  
dbru_label.h                           dbru_label.cxx
dbru_label_sptr.h                      

dbru_object.h                          dbru_object.cxx
dbru_object_sptr.h

dbru_osl.h                             dbru_osl.cxx
dbru_osl_sptr.h

dbru_multiple_instance_object.h        dbru_multiple_instance_object.cxx
dbru_multiple_instance_object_sptr.h
)

aux_source_directory(Templates dbru_sources)

add_library(dbru ${dbru_sources})

target_link_libraries(dbru vsol dbinfo vil_io dbcvr)

add_subdirectory( pro )

# Brown Recognition Tools to aid Testing
add_subdirectory( vis )

# Brown Recognition Correspondence Generator Algorithms
add_subdirectory(algo)

# Brown Recognition Utilities Testing
add_subdirectory( tests )

# Brown Recognition Utilities Processes - new process structure of vxl/contrib/brl/bpro
add_subdirectory( pro2 )


