# Brown shape modeling library
# dbknee/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )


include_directories( ${lems_SOURCE_DIR}/breye1 )

set( dbknee_sources
  dbknee_coord.h       dbknee_coord.cxx        dbknee_coord_sptr.h
  dbknee_tibia_cs.h    dbknee_tibia_cs.cxx
)


aux_source_directory(Templates dbknee_sources)

add_library(dbknee  ${dbknee_sources})

target_link_libraries(dbknee vbl vnl_algo vsl vcsl dbgl_algo dbil_algo dbnl dbmsh3d_algo)

if( BUILD_EXAMPLES )
#  subdirs( examples )
endif( BUILD_EXAMPLES )

# Processing Library
subdirs(pro)

# Visualization Library
subdirs(vis)

if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)
