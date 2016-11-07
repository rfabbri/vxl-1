include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

add_library( btpl_hdr
             btpl_hdr.h btpl_hdr.cxx )

target_link_libraries( btpl_hdr vcl vgl vnl vil )
set( BTPL_HDR_INCLUDE_DIR ${btpl_hdr_SOURCE_DIR} )

add_executable ( btpl_hdr_estimate btpl_hdr_estimate.cxx )
target_link_libraries( btpl_hdr_estimate vcl vil vgl vnl vnl_algo vul btpl_hdr )

add_executable ( btpl_batch_hdr btpl_batch_hdr.cxx )
target_link_libraries( btpl_batch_hdr vcl vil vgl vnl vnl_algo vul btpl_hdr )

add_executable ( btpl_batch_sort btpl_batch_sort.cxx )
target_link_libraries( btpl_batch_sort vcl vil vgl vnl vnl_algo vul btpl_hdr )

add_executable ( btpl_hdr_to_text btpl_hdr_to_text.cxx )
target_link_libraries( btpl_hdr_to_text vcl vil vgl vnl vnl_algo vul btpl_hdr )

add_executable ( btpl_get_hdr_intensity btpl_get_hdr_intensity.cxx btpl_get_shutter.cxx btpl_hdr_hist.cxx btpl_get_variance.cxx)
target_link_libraries( btpl_get_hdr_intensity vcl vil vgl vnl vnl_algo vul btpl_hdr dbsta mbl vifa)
