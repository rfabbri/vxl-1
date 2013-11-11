# This is brcv/seg/dbbgm/algo/CMakeLists.txt
# The Brown Background Modeling Algorithms Library

SET( dbbgm_algo_sources
  dbbgm_hmm_algo.cxx         dbbgm_hmm_algo.h
)

AUX_SOURCE_DIRECTORY(Templates dbbgm_algo_sources)

ADD_LIBRARY( dbbgm_algo  ${dbbgm_algo_sources} )

TARGET_LINK_LIBRARIES( dbbgm_algo dbbgm vil)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



