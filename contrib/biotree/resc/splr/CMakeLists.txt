# /contrib/biotree/resc/splr/CMakeLists.txt
INCLUDE_DIRECTORIES(${biotree_SOURCE_DIR}/resc ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

SET(splr_src 
    splr_populatable_worldpt_roster.h
    splr_splat_resource.h                    splr_splat_resource.txx
    splr_explicit_splat_collection.h         splr_explicit_splat_collection.txx
    splr_map.h                               splr_map.txx
    splr_splat_collection.h
    splr_symmetry.h                          splr_symmetry.cxx 
    splr_pizza_slice_symmetry_sptr.h
    splr_pizza_slice_symmetry.h              splr_pizza_slice_symmetry.cxx
    splr_symmetry_worldpt_roster.h           splr_symmetry_worldpt_roster.cxx
    splr_symmetry_splat_collection.h         splr_symmetry_splat_collection.txx
#    splr_per_orbit_index_splat_collection.h  splr_per_orbit_index_splat_collection.txx
    splr_pizza_slice_symmetry_representatives.h splr_pizza_slice_symmetry_representatives.cxx
    splr_subset_splat_collection.h           splr_subset_splat_collection.txx
    splr_basic_pizza_slice_splat_collection.h splr_basic_pizza_slice_splat_collection.txx
   )
AUX_SOURCE_DIRECTORY(Templates splr_src)
ADD_LIBRARY(splr ${splr_src})

TARGET_LINK_LIBRARIES(splr biob xmvg xmvg_io vgl_io vnl_xio vsl)

#IF(BUILD_TESTING)
#  SUBDIRS( tests )
#ENDIF(BUILD_TESTING)
