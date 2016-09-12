include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic/ )
include_directories( ${lems_SOURCE_DIR}/contrib/ )

set( spatemp_sources
        dbdet_temporal_map.h                                    dbdet_temporal_map.cxx                          dbdet_temporal_map_sptr.h
        dbdet_temporal_map_params.h                             dbdet_temporal_map_params.cxx
        dbdet_temporal_bundle.h                                 dbdet_temporal_bundle.cxx
        dbdet_temporal_model.h                                  dbdet_temporal_model.cxx
        dbdet_temporal_options.h                                dbdet_temporal_options.cxx                      dbdet_temporal_options_sptr.h
        dbdet_contour_fragment.h                                dbdet_contour_fragment.cxx
        dbdet_temporal_utils.h                                  dbdet_temporal_utils.cxx
        dbdet_delaunay_map.h                                    dbdet_delaunay_map.cxx
        dbdet_spatial_temporal_options.h                dbdet_spatial_temporal_options.cxx      dbdet_spatial_temporal_options_sptr.h
        dbdet_region.h                                                  dbdet_region.cxx
        dbdet_spatial_temporal_derivatives.h    dbdet_spatial_temporal_derivatives.cxx
        dbdet_spherical_histogram.h                             dbdet_spherical_histogram.cxx           dbdet_spherical_histogram_sptr.h

)

aux_source_directory(Templates spatemp_sources)
add_library(spatemp_algo ${spatemp_sources})
target_link_libraries(spatemp_algo gc dbnl_algo)

