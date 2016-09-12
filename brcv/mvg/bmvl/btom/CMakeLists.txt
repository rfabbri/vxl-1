# library for tomography
# brl/bmvl/btom/CMakeLists.txt

include( ${LEMS_CMAKE_DIR}/config/vxl_utils.cmake )

include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${lems_SOURCE_DIR}/brcv/mvg/bmvl )
include_directories(${VXL_GEL_INCLUDE_DIR} )

set(btom_sources
    btom_gauss_cylinder.cxx         btom_gauss_cylinder.h  btom_gauss_cylinder_sptr.h
    btom_slice_simulator.cxx        btom_slice_simulator.h
    btom_slice_simulator_params.cxx btom_slice_simulator_params.h
)
aux_source_directory(Templates btom_sources)

add_library(btom ${btom_sources})
install_targets(/lib btom)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/btom ${btom_sources})

target_link_libraries(btom gevd vil1 vnl vbl)

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )
