# brcv/mvl/dbvrl/exe/CMakeLists.txt



include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )


include_directories( ${LEMS_SOURCE_DIR}/contrib)
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/basic )

set( register_images_exe_sources
        register_images_exe.cxx
)

add_executable( register_images_exe  ${register_images_exe_sources})
target_link_libraries(register_images_exe bsta vimt brip vbl vul)

add_executable (compute_homographies_exe compute_homographies_exe.cxx)
target_link_libraries(compute_homographies_exe dbvrl bsta vimt brip vbl vul)

