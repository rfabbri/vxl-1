INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(dborl_pb_edge_det_sources
  dborl_pb_edge_det_params.h
  dborl_pb_edge_det_params_sptr.h
  dborl_pb_edge_det_params.cxx
  dborl_pb_edge_det_main.cxx
  vbl_smart_ptr+dborl_pb_edge_det_params-.cxx
)

ADD_EXECUTABLE (dborl_pb_edge_det ${dborl_pb_edge_det_sources})
TARGET_LINK_LIBRARIES (dborl_pb_edge_det dborl_algo)

SUBDIRS(vox_third_order_edge_det)
SUBDIRS(index_label_test)
