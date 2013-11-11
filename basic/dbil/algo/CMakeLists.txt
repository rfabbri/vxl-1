# This is /basic/dbil/algo/CMakeLists.txt
# The Development Brown Image Algorithms Library 

 
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)

SET( dbil_algo_sources
  dbil_gradient_color.h          dbil_gradient_color.cxx
  dbil_exact_distance_transform.h dbil_exact_distance_transform.cxx
)

IF(OCTAVE_FOUND)
    SET(dbil_algo_sources
        ${dbil_algo_sources}
        dbil_octave.h dbil_octave.cxx
    )
ENDIF(OCTAVE_FOUND)

#AUX_SOURCE_DIRECTORY(Templates dbil_algo_sources)

ADD_LIBRARY( dbil_algo ${dbil_algo_sources} )

TARGET_LINK_LIBRARIES( dbil_algo vil_algo vul)

IF(OCTAVE_FOUND)
    TARGET_LINK_LIBRARIES(dbil_algo dbul)
ENDIF(OCTAVE_FOUND)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
