# breye1/vorl1/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories (${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories(${VXD_UNMAINTAINED_VXDCORE_INCLUDE_DIR})

set(vorl1_sources
                                   vorl1.h
   vorl1_manager.cxx                vorl1_manager.h
)

set(movie_sources
   track.cpp     vorl1_movie_extractor.cxx        track.h 
)


#aux_source_directory(Templates vidpro1_sources)

if( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif( BUILD_EXAMPLES )



add_library(vorl1  ${vorl1_sources})


target_link_libraries(vorl1 vul vidpro1 vidl1 bpro1 vbl vsl)

add_subdirectory(exec)


add_executable (vorl1_frame_extractor vorl1_frame_extractor.cxx)
target_link_libraries(vorl1_frame_extractor vidl1 vil vul)

add_executable (vorl1_movie_extractor ${movie_sources})
target_link_libraries(vorl1_movie_extractor expatpp vidl1 vil vul)
