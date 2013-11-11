# This is contrib/dbvxm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )

PROJECT( dbvxm )

#IF (WIN32 AND VXL_USE_LFS)
##This is a hack since win32 doesn't have transparent Large File Support.
#  ADD_DEFINITIONS( -DBVXM_USE_FSTREAM64 )
#  # so we can include vil_stream_fstream64
#  ADD_DEFINITIONS( -DVIL_USE_FSTREAM64 )
#ENDIF (WIN32 AND VXL_USE_LFS)


SUBDIRS( pro )
SUBDIRS( bin )

SET(dbvxm_sources
    dbvxm_multi_scale_voxel_world.h         dbvxm_multi_scale_voxel_world.cxx
    dbvxm_util.h                            dbvxm_util.cxx  
)

AUX_SOURCE_DIRECTORY(Templates dbvxm_sources)

ADD_LIBRARY(dbvxm ${dbvxm_sources})

TARGET_LINK_LIBRARIES(dbvxm bvxm bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



