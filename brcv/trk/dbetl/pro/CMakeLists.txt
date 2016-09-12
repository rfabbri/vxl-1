# This is brcv/mvg/dbetl/pro/CMakeLists.txt
# Brown Epipolar Curve Processing library

include_directories( ${lems_SOURCE_DIR}/breye1 )

set(dbetl_pro_sources
   dbetl_tracker_process.cxx   dbetl_tracker_process.h
   dbetl_track_storage.cxx     dbetl_track_storage.h     dbetl_track_storage_sptr.h
)

#aux_source_directory(Templates dbetl_pro_sources)

add_library(dbetl_pro ${dbetl_pro_sources})

target_link_libraries(dbetl_pro dbetl dbecl_pro vidpro1)


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)

