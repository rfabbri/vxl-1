set(splr_io_sources
    splr_io_map.hxx                              splr_io_map.h
    splr_io_explicit_splat_collection.hxx        splr_io_explicit_splat_collection.h
    splr_io_explicit_worldpt_roster.cxx          splr_io_explicit_worldpt_roster.h
    splr_io_per_orbit_index_splat_collection.hxx splr_io_per_orbit_index_splat_collection.h
    splr_io_symmetry_splat_collection.hxx        splr_io_per_orbit_index_splat_collection.h
)

aux_source_directory(Templates splr_io_sources)
add_library(splr_io ${splr_io_sources})

target_link_libraries( splr_io xmvg_io xmvg splr  vsl vnl_io vbl vbl_io)

