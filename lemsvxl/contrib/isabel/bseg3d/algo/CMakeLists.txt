# This is contrib/isabel/bseg3d/algo/CMakeLists.txt
# Brown 3D segmentation

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/contrib/isabel )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  
set(bseg3d_algo_sources
  bseg3d_explore_mixtures.h bseg3d_explore_mixtures.cxx
  bseg3d_gmm_l2norm.h       bseg3d_gmm_l2norm.cxx
  bseg3d_convolve_mask.h    bseg3d_convolve_mask.cxx
  bseg3d_merge_mixtures.h   bseg3d_merge_mixtures.cxx
  bseg3d_gauss_multiplier.h bseg3d_gauss_multiplier.cxx
  bseg3d_locate_2d_region.h  bseg3d_locate_2d_region.cxx
)

aux_source_directory(Templates bseg3d_algo_sources)


add_library(bseg3d_algo ${bseg3d_algo_sources})

target_link_libraries(bseg3d_algo bseg3d vnl vbl bsta vil vil_algo vpdl)

# Processes compatible with bprb
add_subdirectory(pro)

# A set of executables
# add_subdirectory(exe)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

