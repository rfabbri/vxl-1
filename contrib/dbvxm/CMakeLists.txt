# This is contrib/dbvxm/CMakeLists.txt

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm)
include_directories( ${lems_SOURCE_DIR}/contrib/dbvxm )

project( dbvxm )

#if (WIN32 AND VXL_USE_LFS)
##This is a hack since win32 doesn't have transparent Large File Support.
#  add_definitions( -DBVXM_USE_FSTREAM64 )
#  # so we can include vil_stream_fstream64
#  add_definitions( -DVIL_USE_FSTREAM64 )
#endif (WIN32 AND VXL_USE_LFS)


add_subdirectory( pro )
add_subdirectory( bin )

set(dbvxm_sources
    dbvxm_multi_scale_voxel_world.h         dbvxm_multi_scale_voxel_world.cxx
    dbvxm_util.h                            dbvxm_util.cxx  
)

aux_source_directory(Templates dbvxm_sources)

add_library(dbvxm ${dbvxm_sources})

target_link_libraries(dbvxm bvxm bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )



