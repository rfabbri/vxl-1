# This is /basic/dbil3d/algo/CMakeLists.txt
# The Development Brown 3d Image Algorithms Library 

include_directories( ${MUL_INCLUDE_DIR} ${VXL_MUL_INCLUDE_DIR})
set( dbil3d_algo_sources
                dbil3d_gauss_filter.h dbil3d_gauss_filter.txx
                dbil3d_finite_differences.h  dbil3d_finite_differences.txx
                dbil3d_finite_second_differences.h  dbil3d_finite_second_differences.txx
                dbil3d_hessian_decompose.h dbil3d_hessian_decompose.cxx dbil3d_hessian_decompose.txx 
                dbil3d_detect_ridges.h dbil3d_detect_ridges.txx dbil3d_detect_ridges.cxx
)

aux_source_directory(Templates dbil3d_algo_sources)

add_library( dbil3d_algo  ${dbil3d_algo_sources} )

target_link_libraries( dbil3d_algo vil3d vil_algo vil3d_algo)

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )
