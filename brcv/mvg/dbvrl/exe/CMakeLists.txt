# brcv/mvl/dbvrl/exe/CMakeLists.txt



INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET( register_images_exe_sources
        register_images_exe.cxx
)

ADD_EXECUTABLE( register_images_exe  ${register_images_exe_sources})
TARGET_LINK_LIBRARIES(register_images_exe bsta vimt brip vbl vul)

ADD_EXECUTABLE (compute_homographies_exe compute_homographies_exe.cxx)
TARGET_LINK_LIBRARIES(compute_homographies_exe dbvrl bsta vimt brip vbl vul)

