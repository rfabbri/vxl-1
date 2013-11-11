# This is brcv/rec/dbcvr/pro/CMakeLists.txt
# Brown curvematch Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )


SET(dbcvr_pro_sources
   dbcvr_curveaverage_process.cxx        dbcvr_curveaverage_process.h
   dbcvr_curvematch_process.cxx          dbcvr_curvematch_process.h
   dbcvr_interp_cvmatch_process.cxx      dbcvr_interp_cvmatch_process.h
   dbcvr_curvematch_storage.cxx         dbcvr_curvematch_storage.h        dbcvr_curvematch_storage_sptr.h
#   dbcvr_mutual_info_process.cxx          dbcvr_mutual_info_process.h
#  dbcvr_region_cor_process.cxx          dbcvr_region_cor_process.h
)


ADD_LIBRARY(dbcvr_pro ${dbcvr_pro_sources})

#TARGET_LINK_LIBRARIES(dbcvr_pro dbcvr dbru vidpro1 brip strk bsta vtol dbsol_algo dbsk2d_pro)
TARGET_LINK_LIBRARIES(dbcvr_pro dbcvr vidpro1 brip vtol dbsol_algo dbsk2d_pro)



