# lemsvxlsrc/contrib/dec/psm/io/CMakeLists.txt

set(psm_io_sources
    psm_io_scene.h  psm_io_scene.cxx
)

aux_source_directory(Templates psm_io_sources)

add_library(psm_io ${psm_io_sources})
target_link_libraries(psm_io psm vsl vbl vil vil_io)


