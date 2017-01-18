# Brown Elastic Curve Matching Library
# brcv/rec/dbcvr/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg)
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXD_BRLD_INCLUDE_DIR})


set(dbcvr_sources

dbcvr_cimatch.h

dbcvr_cvmatch.h
dbcvr_cvmatch.cxx
dbcvr_cvmatch_sptr.h

dbcvr_cvmatch_even.h
dbcvr_cvmatch_even.cxx
dbcvr_cvmatch_even_sptr.h

dbcvr_clsd_cvmatch.h
dbcvr_clsd_cvmatch.cxx
dbcvr_clsd_cvmatch_sptr.h

dbcvr_clsd_cvmatch_even.h
dbcvr_clsd_cvmatch_even.cxx
dbcvr_clsd_cvmatch_even_sptr.h

dbcvr_interp_cvmatch.h
dbcvr_interp_cvmatch.cxx
dbcvr_interp_cvmatch_sptr.h

dbcvr_clsd_interp_cvmatch.h
dbcvr_clsd_interp_cvmatch.cxx
dbcvr_clsd_interp_cvmatch_sptr.h

dbcvr_wsearch.h     
dbcvr_cimfile.cxx         
dbcvr_cimatch.cxx     
dbcvr_wimage.cxx          
dbcvr_wsearch.cxx

dbcvr_cv_cor.h
dbcvr_cv_cor.cxx
dbcvr_cv_cor_sptr.h

#dbcvr_cvmatch_even_3d.h
#dbcvr_cvmatch_even_3d.cxx
#dbcvr_cvmatch_even_3d_sptr.h

dbcvr_open_space_cvmatch_even_grid_cost.h
dbcvr_open_space_cvmatch_even_grid_cost.cxx
dbcvr_open_space_cvmatch_even_grid.h
dbcvr_open_space_cvmatch_even_grid.cxx

dbcvr_open_2d_cvmatch_even_grid_cost.h
dbcvr_open_2d_cvmatch_even_grid_cost.cxx
dbcvr_open_2d_cvmatch_even_grid.h
dbcvr_open_2d_cvmatch_even_grid.cxx
)

aux_source_directory(Templates dbcvr_sources)

add_library(dbcvr  ${dbcvr_sources})

target_link_libraries(dbcvr bsol bsold bgld dbdp bnld_algo vdgl)

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

# Executables
# add_subdirectory(bin)

add_subdirectory(examples)
