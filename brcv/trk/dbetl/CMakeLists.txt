# brcv/trk/dbetl/CMakeLists.txt
# Brown Epipolar Tracking Library

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/mvg)

SET(dbetl_sources
                                   dbetl_image_stats.h 
    dbetl_point_2d.cxx             dbetl_point_2d.h              dbetl_point_2d_sptr.h
    dbetl_point_track.cxx          dbetl_point_track.h           dbetl_point_track_sptr.h
                                   dbetl_camera.h                dbetl_camera_sptr.h
    dbetl_epiprofile.cxx           dbetl_epiprofile.h
    dbetl_tracker.cxx              dbetl_tracker.h
    dbetl_reproject_lsqr_cost.cxx  dbetl_reproject_lsqr_cost.h
)

AUX_SOURCE_DIRECTORY(Templates dbetl_sources)

ADD_LIBRARY(dbetl ${dbetl_sources})

TARGET_LINK_LIBRARIES(dbetl dbecl vil)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
