# brcv/mvl/dbrcl/app/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 ) # DON'T WANT THIS


ADD_EXECUTABLE( dbrcl_camera_app  dbrcl_camera_app.cxx )
TARGET_LINK_LIBRARIES( dbrcl_camera_app dbrcl vpgl vpgl_algo dbinfo dbinfo_pro vnl vgl vil vsol bsta vimt brip vbl vul)

ADD_EXECUTABLE ( dbrcl_unregister_app dbrcl_unregister_app.cxx )
TARGET_LINK_LIBRARIES( dbrcl_unregister_app vcl vgl vnl vpgl vpgl_algo vgl_algo vil vimt vbl vul )

ADD_EXECUTABLE ( dbrcl_reproject_app dbrcl_reproject_app.cxx )
TARGET_LINK_LIBRARIES( dbrcl_reproject_app vcl vgl vnl vil vpgl vpgl_algo vgl_algo dbrcl )
