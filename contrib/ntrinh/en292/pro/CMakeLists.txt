# contrib/ntrinh/pro/CMakeLists.txt
# Process for EN 294 project

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR}/rrel )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR}/vimt )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh/en292 )

SET(en292_pro_sources
  vidpro1_klt_process.cxx               vidpro1_klt_process.h  
  vidpro1_compute_homography_2d_process.cxx         vidpro1_compute_homography_2d_process.h
  vidpro1_homog_2d_storage.cxx        vidpro1_homog_2d_storage.h     vidpro1_homog_2d_storage_sptr.h
  vidpro1_homog_2d_preimage_process.cxx             vidpro1_homog_2d_preimage_process.h
  vidpro1_homog_2d_transform_process.cxx            vidpro1_homog_2d_transform_process.h            
  vidpro1_compute_homog_2d_from_rectangle_process.cxx      vidpro1_compute_homog_2d_from_rectangle_process.h
  vidpro1_homog_2d_transform_image_process.cxx            vidpro1_homog_2d_transform_image_process.h            
)

ADD_LIBRARY(en292_pro  ${en292_pro_sources})

TARGET_LINK_LIBRARIES(en292_pro vidpro1 vil vsol vbl rrel bvgl brip )

