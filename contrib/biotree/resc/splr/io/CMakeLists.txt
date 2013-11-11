SET(splr_io_sources
    splr_io_map.txx                              splr_io_map.h
    splr_io_explicit_splat_collection.txx        splr_io_explicit_splat_collection.h
    splr_io_explicit_worldpt_roster.cxx          splr_io_explicit_worldpt_roster.h
    splr_io_per_orbit_index_splat_collection.txx splr_io_per_orbit_index_splat_collection.h
    splr_io_symmetry_splat_collection.txx        splr_io_per_orbit_index_splat_collection.h
)

AUX_SOURCE_DIRECTORY(Templates splr_io_sources)
ADD_LIBRARY(splr_io ${splr_io_sources})

TARGET_LINK_LIBRARIES( splr_io xmvg_io xmvg splr  vsl vnl_io vbl vbl_io)

