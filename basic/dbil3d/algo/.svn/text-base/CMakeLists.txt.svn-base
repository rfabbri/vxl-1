# This is /basic/dbil3d/algo/CMakeLists.txt
# The Development Brown 3d Image Algorithms Library 

INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} ${VXL_MUL_INCLUDE_DIR})
SET( dbil3d_algo_sources
                dbil3d_gauss_filter.h dbil3d_gauss_filter.txx
                dbil3d_finite_differences.h  dbil3d_finite_differences.txx
                dbil3d_finite_second_differences.h  dbil3d_finite_second_differences.txx
                dbil3d_hessian_decompose.h dbil3d_hessian_decompose.cxx dbil3d_hessian_decompose.txx 
                dbil3d_detect_ridges.h dbil3d_detect_ridges.txx dbil3d_detect_ridges.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbil3d_algo_sources)

ADD_LIBRARY( dbil3d_algo  ${dbil3d_algo_sources} )

TARGET_LINK_LIBRARIES( dbil3d_algo vil3d vil_algo vil3d_algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
