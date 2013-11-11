# breye/vorl/CMakeLists.txt


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET(vorl_sources
                                   vorl.h
   vorl_manager.cxx                vorl_manager.h
)

SET(movie_sources
   track.cpp     vorl_movie_extractor.cxx        track.h 
)


#AUX_SOURCE_DIRECTORY(Templates vidpro_sources)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )



ADD_LIBRARY(vorl  ${vorl_sources})


TARGET_LINK_LIBRARIES(vorl vul vidpro vidl1)

# executables have not been ported from old brown eyes
#SUBDIRS(exec)


ADD_EXECUTABLE (vorl_frame_extractor vorl_frame_extractor.cxx)
TARGET_LINK_LIBRARIES(vorl_frame_extractor vidl1 vil vul)

ADD_EXECUTABLE (vorl_movie_extractor ${movie_sources})
TARGET_LINK_LIBRARIES(vorl_movie_extractor expatpp vidl1 vil vul)
