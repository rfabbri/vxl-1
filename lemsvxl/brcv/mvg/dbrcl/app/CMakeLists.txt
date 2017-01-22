# brcv/mvl/dbrcl/app/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc )

include_directories( ${lems_SOURCE_DIR}/contrib)
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/breye1 ) # DON'T WANT THIS


add_executable( dbrcl_camera_app  dbrcl_camera_app.cxx )
target_link_libraries( dbrcl_camera_app dbrcl vpgl vpgl_algo dbinfo dbinfo_pro vnl vgl vil vsol bsta vimt brip vbl vul)

add_executable ( dbrcl_unregister_app dbrcl_unregister_app.cxx )
target_link_libraries( dbrcl_unregister_app bpgl_algo vcl vgl vnl vpgl vpgl_algo vgl_algo vil vimt vbl vul )

add_executable ( dbrcl_reproject_app dbrcl_reproject_app.cxx )
target_link_libraries( dbrcl_reproject_app vcl vgl vnl vil vpgl vpgl_algo vgl_algo dbrcl )
