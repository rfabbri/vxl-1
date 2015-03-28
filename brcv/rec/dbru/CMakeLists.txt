# brcv/rec/dbru/CMakeLists.txt
# Brown Recognition Utililities Libraries.

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/rec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(dbru_sources  
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

AUX_SOURCE_DIRECTORY(Templates dbru_sources)

ADD_LIBRARY(dbru ${dbru_sources})

TARGET_LINK_LIBRARIES(dbru vsol dbinfo vil_io dbcvr)

SUBDIRS( pro )

# Brown Recognition Tools to aid Testing
SUBDIRS( vis )

# Brown Recognition Correspondence Generator Algorithms
SUBDIRS (algo)

# Brown Recognition Utilities Testing
SUBDIRS( tests )

# Brown Recognition Utilities Processes - new process structure of vxl/contrib/brl/bpro
SUBDIRS( pro2 )


