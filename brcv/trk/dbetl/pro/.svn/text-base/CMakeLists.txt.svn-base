# This is brcv/mvg/dbetl/pro/CMakeLists.txt
# Brown Epipolar Curve Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbetl_pro_sources
   dbetl_tracker_process.cxx   dbetl_tracker_process.h
   dbetl_track_storage.cxx     dbetl_track_storage.h     dbetl_track_storage_sptr.h
)

#AUX_SOURCE_DIRECTORY(Templates dbetl_pro_sources)

ADD_LIBRARY(dbetl_pro ${dbetl_pro_sources})

TARGET_LINK_LIBRARIES(dbetl_pro dbetl vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

