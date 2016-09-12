include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )

add_library( dbrcl 
             dbrcl_point_tracker.h                 dbrcl_point_tracker.cxx
             dbrcl_compute_constrained_cameras.h   dbrcl_compute_constrained_cameras.cxx
             dbrcl_compute_cameras.h               dbrcl_compute_cameras.cxx )

target_link_libraries( dbrcl vcl vgl vnl vil vgl_algo vil_algo vsol vpgl vpgl_algo vgel dbinfo mvl )
set( dbrcl_INCLUDE_DIR ${dbrcl_SOURCE_DIR} )

subdirs( app )
subdirs( tests )
