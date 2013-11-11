# This is brl/blem/brcv/trk/dbinfo/pro/CMakeLists.txt
# Brown Mutual Information Tracking Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbinfo_pro_sources
                                           dbinfo_track_storage_sptr.h
    dbinfo_track_storage.cxx               dbinfo_track_storage.h
                                           dbinfo_osl_storage_sptr.h
    dbinfo_osl_storage.cxx                 dbinfo_osl_storage.h

    dbinfo_multi_track_process.cxx         dbinfo_multi_track_process.h
    dbinfo_load_tracks_process.cxx         dbinfo_load_tracks_process.h
    dbinfo_save_tracks_process.cxx         dbinfo_save_tracks_process.h
    dbinfo_refine_tracks_process.cxx       dbinfo_refine_tracks_process.h
    dbinfo_multi_poly_track_process.cxx    dbinfo_multi_poly_track_process.h
    dbinfo_load_vj_polys_process.cxx       dbinfo_load_vj_polys_process.h
    dbinfo_imatch_minfo_process.cxx        dbinfo_imatch_minfo_process.h
    dbinfo_load_osl_process.cxx            dbinfo_load_osl_process.h
    dbinfo_save_osl_process.cxx            dbinfo_save_osl_process.h
)

ADD_LIBRARY(dbinfo_pro ${dbinfo_pro_sources})

TARGET_LINK_LIBRARIES(dbinfo_pro dbinfo vidpro1)

#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)

