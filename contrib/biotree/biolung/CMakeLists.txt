# 3D realisitc lung modeling library
# algo/biolung/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )

set(biolung_sources

biolung_3D_Tree.h
biolung_3d_Tree.cxx
biolung_lungmodel.h
biolung_lungmodel.cxx

)

aux_source_directory(Templates biolung_sources)

add_library(biolung  ${biolung_sources})

target_link_libraries( biolung vgl vcl vnl vbl vil)

subdirs(pro)

if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)

