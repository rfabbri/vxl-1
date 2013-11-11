# Brown Elastic Curve Matching Library
# brcv/rec/dbcvr/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})

SET(dbcvr_sources

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

AUX_SOURCE_DIRECTORY(Templates dbcvr_sources)

ADD_LIBRARY(dbcvr  ${dbcvr_sources})

TARGET_LINK_LIBRARIES(dbcvr bsol dbsol dbdp dbnl_algo vdgl)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

# Executables
# SUBDIRS(bin)

SUBDIRS(examples)
