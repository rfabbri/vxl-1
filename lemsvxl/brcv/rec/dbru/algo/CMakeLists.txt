# brcv/rec/dbru/algo/CMakeLists.txt
# Brown Recognition Utililities Algorithms Libraries.


include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/rec )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(dbru_algo_sources  
dbru_rcor.h                            dbru_rcor.cxx
dbru_rcor_sptr.h

dbru_rcor_generator.h                  dbru_rcor_generator.cxx
)

aux_source_directory(Templates dbru_sources)

add_library(dbru_algo ${dbru_algo_sources})

target_link_libraries(dbru_algo dbru dbskr dbsk2d_algo mbl)
