include_directories(${biotree_SOURCE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS -O3)
#set(CMAKE_CXX_FLAGS -g)

set(bioproj_src
    bioproj_io.h                bioproj_io.cxx
    bioproj_mem_io.h            bioproj_mem_io.cxx
    bioproj_proc.h              bioproj_proc.cxx
    bioproj_mem_proc.h          bioproj_mem_proc.cxx
    bioproj_data.h              bioproj_data.cxx
#    bioproj_mem_data.h          bioproj_mem_data.cxx
    bioproj_nu_g_filter.h       bioproj_nu_g_filter.cxx
    bioproj_nu_gx_filter.h      bioproj_nu_gx_filter.cxx
    bioproj_nu_gy_filter.h      bioproj_nu_gy_filter.cxx
    bioproj_nu_gxx_filter.h     bioproj_nu_gxx_filter.cxx
    bioproj_nu_gyy_filter.h     bioproj_nu_gyy_filter.cxx
    bioproj_nu_gxy_filter.h     bioproj_nu_gxy_filter.cxx
    bioproj_g_filter.h          bioproj_g_filter.cxx
    bioproj_gz_filter.h         bioproj_gz_filter.cxx
    bioproj_gzz_filter.h        bioproj_gzz_filter.cxx
    error_functions.h           error_functions.cxx
    bioproj_filter_integrant.h  
   )

aux_source_directory(Templates bioproj_src)

add_library(bioproj ${bioproj_src})

target_link_libraries(bioproj vcl  vgl xscan imgr_file_format vil3d bscs vcsl) 

if(BUILD_EXAMPLES)
    subdirs(examples)
endif(BUILD_EXAMPLES)
  
#if(BUILD_TESTING)
#  subdirs(tests)
#endif(BUILD_TESTING)

if(SIMVOLEON_FOUND)
  subdirs(vis)
endif(SIMVOLEON_FOUND)
