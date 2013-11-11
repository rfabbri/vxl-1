# This is brcv/mvg/dvpgl/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET( dvpgl_pro_sources
   dvpgl_fm_storage.cxx  dvpgl_fm_storage.h  dvpgl_fm_storage_sptr.h
   dvpgl_camera_storage.cxx  dvpgl_camera_storage.h  dvpgl_camera_storage_sptr.h
)


ADD_LIBRARY(dvpgl_pro ${dvpgl_pro_sources})

TARGET_LINK_LIBRARIES(dvpgl_pro vpgl vidpro1 )


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

