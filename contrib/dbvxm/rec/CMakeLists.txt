# This is contrib/dbvxm/CMakeLists.txt

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm)
include_directories( ${lems_SOURCE_DIR}/contrib/dbvxm )
include_directories( ${lems_SOURCE_DIR}/basic )

add_subdirectory( pro )

set(dbvxm_rec_sources
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

aux_source_directory(Templates dbvxm_rec_sources)

add_library(dbvxm_rec ${dbvxm_rec_sources})

target_link_libraries(dbvxm_rec bsta_algo dbgrl bsta bvgl vil brip)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )



