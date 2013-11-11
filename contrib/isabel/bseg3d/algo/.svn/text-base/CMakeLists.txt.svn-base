# This is contrib/isabel/bseg3d/algo/CMakeLists.txt
# Brown 3D segmentation

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/isabel )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  
SET(bseg3d_algo_sources
  bseg3d_explore_mixtures.h bseg3d_explore_mixtures.cxx
  bseg3d_gmm_l2norm.h       bseg3d_gmm_l2norm.cxx
  bseg3d_convolve_mask.h    bseg3d_convolve_mask.cxx
  bseg3d_merge_mixtures.h   bseg3d_merge_mixtures.cxx
  bseg3d_gauss_multiplier.h bseg3d_gauss_multiplier.cxx
  bseg3d_locate_2d_region.h  bseg3d_locate_2d_region.cxx
)

AUX_SOURCE_DIRECTORY(Templates bseg3d_algo_sources)


ADD_LIBRARY(bseg3d_algo ${bseg3d_algo_sources})

TARGET_LINK_LIBRARIES(bseg3d_algo bseg3d vnl vbl bsta vil vil_algo vpdl)

# Processes compatible with bprb
SUBDIRS(pro)

# A set of executables
# SUBDIRS(exe)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

