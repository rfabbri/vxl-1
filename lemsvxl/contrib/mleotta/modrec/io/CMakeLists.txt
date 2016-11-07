# contrib/mleotta/modrec/io/CMakeLists.txt
# Binary IO for modrec


set(modrec_io_sources
   modrec_io_desc_feature_3d.hxx   modrec_io_desc_feature_3d.h 
   modrec_io_codeword.hxx          modrec_io_codeword.h
)

aux_source_directory(Templates modrec_io_sources)

add_library(modrec_io  ${modrec_io_sources})

target_link_libraries(modrec_io modrec vnl_io vgl_io)

if( BUILD_TESTING )
#  add_subdirectory(tests)
endif( BUILD_TESTING )
