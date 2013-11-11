# breye1/vorl1/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )

INCLUDE_DIRECTORIES (${VXL_BRL_INCLUDE_DIR}/b3p )

SET(vorl1_sources
                                   vorl1.h
   vorl1_manager.cxx                vorl1_manager.h
)

SET(movie_sources
   track.cpp     vorl1_movie_extractor.cxx        track.h 
)


#AUX_SOURCE_DIRECTORY(Templates vidpro1_sources)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )



ADD_LIBRARY(vorl1  ${vorl1_sources})


TARGET_LINK_LIBRARIES(vorl1 vul vidpro1 vidl1)

SUBDIRS(exec)


ADD_EXECUTABLE (vorl1_frame_extractor vorl1_frame_extractor.cxx)
TARGET_LINK_LIBRARIES(vorl1_frame_extractor vidl1 vil vul)

ADD_EXECUTABLE (vorl1_movie_extractor ${movie_sources})
TARGET_LINK_LIBRARIES(vorl1_movie_extractor expatpp vidl1 vil vul)
