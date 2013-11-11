INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )

ADD_LIBRARY( dbrcl 
             dbrcl_point_tracker.h                 dbrcl_point_tracker.cxx
             dbrcl_compute_constrained_cameras.h   dbrcl_compute_constrained_cameras.cxx
             dbrcl_compute_cameras.h               dbrcl_compute_cameras.cxx )

TARGET_LINK_LIBRARIES( dbrcl vcl vgl vnl vil vgl_algo vil_algo vsol vpgl vpgl_algo vgel dbinfo mvl )
SET( dbrcl_INCLUDE_DIR ${dbrcl_SOURCE_DIR} )

SUBDIRS( app )
SUBDIRS( tests )
