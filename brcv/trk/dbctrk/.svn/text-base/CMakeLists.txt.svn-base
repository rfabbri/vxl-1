# Brown curve tracking library
# brcv/trk/dbctrk/CMakeLists.txt
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

SET(dbctrk_sources
   
    dbctrk_algs.cxx                    dbctrk_algs.h
    dbctrk_curve_description.cxx       dbctrk_curve_description.h
    dbctrk_curve_tracker_primitive.cxx dbctrk_curve_tracker_primitive.h
    dbctrk_tracker_curve.cxx           dbctrk_tracker_curve.h  dbctrk_tracker_curve_sptr.h
    dbctrk_curve_tracking.cxx          dbctrk_curve_tracking.h
    dbctrk_curve_matching.cxx          dbctrk_curve_matching.h
    dbctrk_curve_clustering.cxx        dbctrk_curve_clustering.h
    dbctrk_curve.cxx                   dbctrk_curve.h
    dbctrk_DPMatch.cxx                 dbctrk_DPMatch.h
                                       dbctrk_curveMatch.h
    dbctrk_utils.cxx                   dbctrk_utils.h
    dbctrk_stereo_matching.cxx         dbctrk_stereo_matching.h
    dbctrk_stereo_reconstruction.cxx   dbctrk_stereo_reconstruction.h
    dbctrk_benchmarking.cxx            dbctrk_benchmarking.h
 
)
AUX_SOURCE_DIRECTORY(Templates dbctrk_sources)

ADD_LIBRARY(dbctrk  ${dbctrk_sources})

TARGET_LINK_LIBRARIES(dbctrk vbl bdgl btol vsol vil vnl mvl mbl brct  vsl dbspi)


SUBDIRS(pro)
IF (VXL_VGUI_FOUND)
    SUBDIRS(vis)
ENDIF (VXL_VGUI_FOUND)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

