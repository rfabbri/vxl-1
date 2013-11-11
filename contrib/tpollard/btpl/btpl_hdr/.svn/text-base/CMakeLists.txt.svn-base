INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

ADD_LIBRARY( btpl_hdr
             btpl_hdr.h btpl_hdr.cxx )

TARGET_LINK_LIBRARIES( btpl_hdr vcl vgl vnl vil )
SET( BTPL_HDR_INCLUDE_DIR ${btpl_hdr_SOURCE_DIR} )

ADD_EXECUTABLE ( btpl_hdr_estimate btpl_hdr_estimate.cxx )
TARGET_LINK_LIBRARIES( btpl_hdr_estimate vcl vil vgl vnl vnl_algo vul btpl_hdr )

ADD_EXECUTABLE ( btpl_batch_hdr btpl_batch_hdr.cxx )
TARGET_LINK_LIBRARIES( btpl_batch_hdr vcl vil vgl vnl vnl_algo vul btpl_hdr )

ADD_EXECUTABLE ( btpl_batch_sort btpl_batch_sort.cxx )
TARGET_LINK_LIBRARIES( btpl_batch_sort vcl vil vgl vnl vnl_algo vul btpl_hdr )

ADD_EXECUTABLE ( btpl_hdr_to_text btpl_hdr_to_text.cxx )
TARGET_LINK_LIBRARIES( btpl_hdr_to_text vcl vil vgl vnl vnl_algo vul btpl_hdr )

ADD_EXECUTABLE ( btpl_get_hdr_intensity btpl_get_hdr_intensity.cxx btpl_get_shutter.cxx btpl_hdr_hist.cxx btpl_get_variance.cxx)
TARGET_LINK_LIBRARIES( btpl_get_hdr_intensity vcl vil vgl vnl vnl_algo vul btpl_hdr dbsta mbl vifa)
