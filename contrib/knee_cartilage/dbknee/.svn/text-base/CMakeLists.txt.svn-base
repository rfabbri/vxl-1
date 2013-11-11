# Brown shape modeling library
# dbknee/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET( dbknee_sources
  dbknee_coord.h       dbknee_coord.cxx        dbknee_coord_sptr.h
  dbknee_tibia_cs.h    dbknee_tibia_cs.cxx
)


AUX_SOURCE_DIRECTORY(Templates dbknee_sources)

ADD_LIBRARY(dbknee  ${dbknee_sources})

TARGET_LINK_LIBRARIES(dbknee vbl vnl_algo vsl vcsl dbgl_algo dbil_algo dbnl dbmsh3d_algo)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
