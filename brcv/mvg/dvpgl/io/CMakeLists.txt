# This is brcv/mvg/dvpgl/io/CMakeLists.txt


SET(dvpgl_io_sources
#    dpgl_io_camera.h               vpgl_io_camera.txx      vpgl_io_camera.cxx
#    dpgl_io_proj_camera.h             vpgl_io_proj_camera.txx
#    dpgl_io_perspective_camera.h      vpgl_io_perspective_camera.txx
    dvpgl_io_cameras.h   dvpgl_io_cameras.txx
)

AUX_SOURCE_DIRECTORY(Templates dvpgl_io_sources)

ADD_LIBRARY(dvpgl_io ${dvpgl_io_sources})

TARGET_LINK_LIBRARIES(dvpgl_io vpgl_io vsl vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
