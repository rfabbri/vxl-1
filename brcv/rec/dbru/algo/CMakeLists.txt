# brcv/rec/dbru/algo/CMakeLists.txt
# Brown Recognition Utililities Algorithms Libraries.


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/rec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(dbru_algo_sources  
dbru_rcor_generator.h                  dbru_rcor_generator.cxx
dbru_object_matcher.h                  dbru_object_matcher.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbru_sources)

ADD_LIBRARY(dbru_algo ${dbru_algo_sources})

TARGET_LINK_LIBRARIES(dbru_algo dbru dbskr dbsk2d_algo mbl)
