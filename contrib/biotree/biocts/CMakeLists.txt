# CT simulation library
# algo/biocts/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )

set(biocts_sources

biocts_rvgs.h
biocts_rvgs.cxx
biocts_rngs.h
biocts_rngs.cxx
biocts_Volume3D.h
biocts_Volume3D.cxx
biocts_ctsim.h
biocts_ctsim.cxx

)

aux_source_directory(Templates biocts_sources)

add_library(biocts  ${biocts_sources})

target_link_libraries( biocts vgl vcl vnl vbl vil)

subdirs(pro)

if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)
