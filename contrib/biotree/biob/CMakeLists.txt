# /contrib/biotree/biob/CMakeLists.txt
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

SET(biob_src 
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
    biob_worldpt_field.h biob_worldpt_field.txx
    biob_enumerator.h biob_enumerator.txx
    biob_roster_to_grid_mapping.h            biob_roster_to_grid_mapping.cxx
)  

AUX_SOURCE_DIRECTORY(Templates biob_src)

ADD_LIBRARY(biob ${biob_src})

TARGET_LINK_LIBRARIES(biob vgl vgl_xio vbl vsl)

IF(BUILD_TESTING)
  SUBDIRS( tests )
ENDIF(BUILD_TESTING)
