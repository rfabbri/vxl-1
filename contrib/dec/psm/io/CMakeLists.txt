# lemsvxlsrc/contrib/dec/psm/io/CMakeLists.txt

SET(psm_io_sources
    psm_io_scene.h  psm_io_scene.cxx
)

AUX_SOURCE_DIRECTORY(Templates psm_io_sources)

ADD_LIBRARY(psm_io ${psm_io_sources})
TARGET_LINK_LIBRARIES(psm_io psm vsl vbl vil vil_io)


