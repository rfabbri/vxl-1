# This is brcv/mvg/dbecl/pro/CMakeLists.txt
# Brown Epipolar Curve Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbecl_pro_sources
   dbecl_builder_process.cxx   dbecl_builder_process.h
   dbecl_episeg_storage.cxx    dbecl_episeg_storage.h     dbecl_episeg_storage_sptr.h
)

#AUX_SOURCE_DIRECTORY(Templates dbecl_pro_sources)

ADD_LIBRARY(dbecl_pro ${dbecl_pro_sources})

TARGET_LINK_LIBRARIES(dbecl_pro dbecl vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

