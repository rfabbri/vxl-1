# /contrib/biotree/biob/CMakeLists.txt
include_directories(${LEMS_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

set(biob_src 
    biob_worldpt_transformation.h            biob_worldpt_transformation.cxx
    biob_grid_worldpt_roster.h               biob_grid_worldpt_roster.cxx       
    biob_worldpt_index.h                     biob_worldpt_index.cxx                
    biob_subset_worldpt_roster.h             biob_subset_worldpt_roster.cxx
    biob_subset_worldpt_roster_sptr.h
    biob_find_roster_bounding_box.h          biob_find_roster_bounding_box.cxx 
    biob_worldpt_neighborhood_structure.h
    biob_explicit_neighborhood_structure.cxx biob_explicit_neighborhood_structure.h
    biob_explicit_worldpt_roster.cxx biob_explicit_worldpt_roster.h
    biob_worldpt_roster.h                    biob_worldpt_roster.cxx
    biob_worldpt_roster_sptr.h
    biob_subset_worldpt_roster_sptr.h
    biob_worldpt_field.h biob_worldpt_field.hxx
    biob_enumerator.h biob_enumerator.hxx
    biob_roster_to_grid_mapping.h            biob_roster_to_grid_mapping.cxx
)  

aux_source_directory(Templates biob_src)

add_library(biob ${biob_src})

target_link_libraries(biob vgl vgl_xio vbl vsl)

if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
