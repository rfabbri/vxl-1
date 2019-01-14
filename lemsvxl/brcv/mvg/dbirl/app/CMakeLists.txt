# brcv/mvl/dbirl/app/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib)
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set( dbirl_app_sources dbirl_app.cxx )

add_executable( dbirl_app  ${dbirl_app_sources} )
target_link_libraries( dbirl_app dbirl dbinfo_pro dbinfo vnl vgl vsol bsta vimt brip vbl vul)
