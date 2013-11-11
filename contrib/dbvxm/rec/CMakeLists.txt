# This is contrib/dbvxm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SUBDIRS( pro )

SET(dbvxm_rec_sources
    dbvxm_bayesian_propagation.h  dbvxm_bayesian_propagation.cxx
    dbvxm_param_estimation.h  dbvxm_param_estimation.cxx
    dbvxm_fg_bg_pair_density.h  dbvxm_fg_bg_pair_density.cxx
    dbvxm_fg_pair_density.h  dbvxm_fg_pair_density.cxx
    dbvxm_pair_density.h          dbvxm_pair_density.cxx
    dbvxm_bg_pair_density.h  dbvxm_bg_pair_density.cxx
    dbvxm_part_base.h     dbvxm_part_base.cxx
    dbvxm_part_base_sptr.h
    dbvxm_part_gaussian.h    dbvxm_part_gaussian.cxx
    dbvxm_part_gaussian_sptr.h
    dbvxm_part_hierarchy.h    dbvxm_part_hierarchy.cxx
    dbvxm_part_hierarchy_sptr.h
    dbvxm_hierarchy_edge.h    dbvxm_hierarchy_edge.cxx
    dbvxm_hierarchy_edge_sptr.h
    dbvxm_part_hierarchy_builder.h    dbvxm_part_hierarchy_builder.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbvxm_rec_sources)

ADD_LIBRARY(dbvxm_rec ${dbvxm_rec_sources})

TARGET_LINK_LIBRARIES(dbvxm_rec bsta_algo dbgrl bsta bvgl vil brip)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



