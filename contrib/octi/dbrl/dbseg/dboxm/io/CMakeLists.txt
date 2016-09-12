# brl/bseg/boxm/io/CMakeLists.txt

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

if(EXPAT_FOUND)
include_directories (${EXPAT_INCLUDE_DIRS} )
include_directories (${BRL_INCLUDE_DIR}/b3p/expatpp )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(boxm_io_sources
   boxm_io_scene_base.cxx   boxm_io_scene_base.h
)

aux_source_directory(Templates boxm_io_sources)

add_library(boxm_io ${boxm_io_sources})

target_link_libraries(boxm_io boxm ${EXPAT_LIBRARIES} expatpp)
if ( APPLE )
  target_link_libraries(boxm_io expat)
endif ( APPLE )
endif(EXPAT_FOUND)

