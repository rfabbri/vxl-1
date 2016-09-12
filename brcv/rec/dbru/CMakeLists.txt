# brcv/rec/dbru/CMakeLists.txt
# Brown Recognition Utililities Libraries.

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/rec )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_RPL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(dbru_sources  
dbru_label.h                           dbru_label.cxx
dbru_label_sptr.h                      

dbru_object.h                          dbru_object.cxx
dbru_object_sptr.h

dbru_rcor.h                            dbru_rcor.cxx
dbru_rcor_sptr.h

dbru_osl.h                             dbru_osl.cxx
dbru_osl_sptr.h

dbru_multiple_instance_object.h        dbru_multiple_instance_object.cxx
dbru_multiple_instance_object_sptr.h
)

aux_source_directory(Templates dbru_sources)

add_library(dbru ${dbru_sources})

target_link_libraries(dbru vsol dbinfo vil_io dbcvr)

subdirs( pro )

# Brown Recognition Tools to aid Testing
subdirs( vis )

# Brown Recognition Correspondence Generator Algorithms
subdirs (algo)

# Brown Recognition Utilities Testing
subdirs( tests )

# Brown Recognition Utilities Processes - new process structure of vxl/contrib/brl/bpro
subdirs( pro2 )


