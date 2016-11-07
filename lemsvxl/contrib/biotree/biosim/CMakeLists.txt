include_directories(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/brcv/mvg ${lems_SOURCE_DIR}/basic/dbsta2)
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc ${VXL_MUL_INCLUDE_DIR})

set(biosim_src
biosim_ctsim.h              biosim_ctsim.cxx
biosim_cylinder_pyramid.h   biosim_cylinder_pyramid.cxx
biosim_deformed_torus.h     biosim_deformed_torus.cxx
)

aux_source_directory(Templates biosim_src)
add_library(biosim ${biosim_src})
target_link_libraries(biosim xmvg xscan biocts vpgl vpgl_algo vgl vil vil3d vbl vnl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif (BUILD_EXAMPLES)
