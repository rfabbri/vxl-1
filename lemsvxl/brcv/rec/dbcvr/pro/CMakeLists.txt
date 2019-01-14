# This is brcv/rec/dbcvr/pro/CMakeLists.txt
# Brown curvematch Processing library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )

include_directories(${BRL_INCLUDE_DIR}/bseg )
include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR} )


set(dbcvr_pro_sources
   dbcvr_curveaverage_process.cxx        dbcvr_curveaverage_process.h
   dbcvr_curvematch_process.cxx          dbcvr_curvematch_process.h
   dbcvr_interp_cvmatch_process.cxx      dbcvr_interp_cvmatch_process.h
   dbcvr_curvematch_storage.cxx         dbcvr_curvematch_storage.h        dbcvr_curvematch_storage_sptr.h
#   dbcvr_mutual_info_process.cxx          dbcvr_mutual_info_process.h
#  dbcvr_region_cor_process.cxx          dbcvr_region_cor_process.h
)


add_library(dbcvr_pro ${dbcvr_pro_sources})

#target_link_libraries(dbcvr_pro dbcvr dbru vidpro1 brip strk bsta vtol bsold_algo dbsk2d_pro)
target_link_libraries(dbcvr_pro dbcvr vidpro1 brip vtol bsold_algo dbsk2d_pro)



