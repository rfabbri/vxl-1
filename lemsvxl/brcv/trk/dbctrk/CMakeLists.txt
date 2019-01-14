# Brown curve tracking library
# brcv/trk/dbctrk/CMakeLists.txt
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${LEMS_SOURCE_DIR}/brcv/mvg/bmvl )
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

set(dbctrk_sources
   
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
aux_source_directory(Templates dbctrk_sources)

add_library(dbctrk  ${dbctrk_sources})

target_link_libraries(dbctrk vbl bdgl btol vsol vil vnl mvl mbl brct bspid vsl)


add_subdirectory(pro)
if (VXL_VGUI_FOUND)
    add_subdirectory(vis)
endif (VXL_VGUI_FOUND)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

