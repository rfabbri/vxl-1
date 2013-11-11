INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )

SET( spatemp_sources
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

AUX_SOURCE_DIRECTORY(Templates spatemp_sources)
ADD_LIBRARY(spatemp_algo ${spatemp_sources})
TARGET_LINK_LIBRARIES(spatemp_algo gc dbnl_algo)

