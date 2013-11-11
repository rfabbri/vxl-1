INCLUDE_DIRECTORIES(${biotree_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})

SET(CMAKE_CXX_FLAGS -O3)
#SET(CMAKE_CXX_FLAGS -g)

SET(bioproj_src
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

AUX_SOURCE_DIRECTORY(Templates bioproj_src)

ADD_LIBRARY(bioproj ${bioproj_src})

TARGET_LINK_LIBRARIES(bioproj vcl  vgl xscan imgr_file_format vil3d bscs vcsl) 

IF(BUILD_EXAMPLES)
    SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)
  
#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)

IF(SIMVOLEON_FOUND)
  SUBDIRS(vis)
ENDIF(SIMVOLEON_FOUND)
