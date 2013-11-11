# brcv/trk/dbinfo/CMakeLists.txt
# brcv mutual information tracking library

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )

SET(dbinfo_sources
   dbinfo_region_geometry.h        dbinfo_region_geometry.cxx    
   dbinfo_region_geometry_sptr.h  
   dbinfo_feature_base.h           dbinfo_feature_base.cxx
   dbinfo_feature_base_sptr.h          
   dbinfo_feature_data_base.h      dbinfo_feature_data.txx
   dbinfo_feature_data_base_sptr.h 
   dbinfo_feature_data.h
   dbinfo_feature_format.h         dbinfo_feature_format.cxx 
   dbinfo_intensity_feature.cxx       dbinfo_intensity_feature.h     
                                       dbinfo_gradient_feature_sptr.h     
    dbinfo_gradient_feature.cxx        dbinfo_gradient_feature.h     
                                       dbinfo_ihs_feature_sptr.h     
    dbinfo_ihs_feature.cxx             dbinfo_ihs_feature.h     
                                       dbinfo_observation_sptr.h
    dbinfo_observation.cxx             dbinfo_observation.h
    dbinfo_observation_generator.cxx   dbinfo_observation_generator.h
                                       dbinfo_observation_matcher_sptr.h
    dbinfo_observation_matcher.cxx     dbinfo_observation_matcher.h
    dbinfo_object_matcher.cxx          dbinfo_object_matcher.h
                                       dbinfo_osl_sptr.h
    dbinfo_osl.cxx                     dbinfo_osl.h
                                   dbinfo_track_sptr.h
    dbinfo_track.cxx                   dbinfo_track.h
                                       dbinfo_track_geometry_sptr.h
    dbinfo_track_geometry.cxx          dbinfo_track_geometry.h
                                       dbinfo_mi_track_sptr.h
    dbinfo_mi_track.cxx                dbinfo_mi_track.h
    dbinfo_multi_tracker_params.cxx    dbinfo_multi_tracker_params.h
    dbinfo_multi_tracker.cxx           dbinfo_multi_tracker.h
                                       dbinfo_match_optimizer_sptr.h
    dbinfo_match_optimizer.cxx         dbinfo_match_optimizer.h
)

AUX_SOURCE_DIRECTORY(Templates dbinfo_sources)

ADD_LIBRARY(dbinfo ${dbinfo_sources})

TARGET_LINK_LIBRARIES(dbinfo sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
