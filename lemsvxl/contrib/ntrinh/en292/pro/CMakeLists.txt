# contrib/ntrinh/pro/CMakeLists.txt
# Process for EN 294 project

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR}/rrel )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR}/vimt )
include_directories( ${VXL_BRL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

include_directories( ${LEMS_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${LEMS_SOURCE_DIR}/contrib/ntrinh/en292 )

set(en292_pro_sources
  vidpro1_klt_process.cxx               vidpro1_klt_process.h  
  vidpro1_compute_homography_2d_process.cxx         vidpro1_compute_homography_2d_process.h
  vidpro1_homog_2d_storage.cxx        vidpro1_homog_2d_storage.h     vidpro1_homog_2d_storage_sptr.h
  vidpro1_homog_2d_preimage_process.cxx             vidpro1_homog_2d_preimage_process.h
  vidpro1_homog_2d_transform_process.cxx            vidpro1_homog_2d_transform_process.h            
  vidpro1_compute_homog_2d_from_rectangle_process.cxx      vidpro1_compute_homog_2d_from_rectangle_process.h
  vidpro1_homog_2d_transform_image_process.cxx            vidpro1_homog_2d_transform_image_process.h            
)

add_library(en292_pro  ${en292_pro_sources})

target_link_libraries(en292_pro vidpro1 vil vsol vbl rrel bvgl brip )

