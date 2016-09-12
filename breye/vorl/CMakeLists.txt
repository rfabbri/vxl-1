# breye/vorl/CMakeLists.txt


include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )

set(vorl_sources
                                   vorl.h
   vorl_manager.cxx                vorl_manager.h
)

set(movie_sources
   track.cpp     vorl_movie_extractor.cxx        track.h 
)


#aux_source_directory(Templates vidpro_sources)

if( BUILD_EXAMPLES )
  subdirs( examples )
endif( BUILD_EXAMPLES )



add_library(vorl  ${vorl_sources})


target_link_libraries(vorl vul vidpro vidl1)

# executables have not been ported from old brown eyes
#subdirs(exec)


add_executable (vorl_frame_extractor vorl_frame_extractor.cxx)
target_link_libraries(vorl_frame_extractor vidl1 vil vul)

add_executable (vorl_movie_extractor ${movie_sources})
target_link_libraries(vorl_movie_extractor expatpp vidl1 vil vul)
