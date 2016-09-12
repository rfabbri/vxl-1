# brcv/trk/dbetl/CMakeLists.txt
# Brown Epipolar Tracking Library

include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${lems_SOURCE_DIR}/brcv/mvg)

set(dbetl_sources
                                   dbetl_image_stats.h 
    dbetl_point_2d.cxx             dbetl_point_2d.h              dbetl_point_2d_sptr.h
    dbetl_point_track.cxx          dbetl_point_track.h           dbetl_point_track_sptr.h
                                   dbetl_camera.h                dbetl_camera_sptr.h
    dbetl_epiprofile.cxx           dbetl_epiprofile.h
    dbetl_tracker.cxx              dbetl_tracker.h
    dbetl_reproject_lsqr_cost.cxx  dbetl_reproject_lsqr_cost.h
)

aux_source_directory(Templates dbetl_sources)

add_library(dbetl ${dbetl_sources})

target_link_libraries(dbetl dbecl vil)

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
