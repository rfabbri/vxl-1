# This is brcv/seg/dbbgm/algo/CMakeLists.txt
# The Brown Background Modeling Algorithms Library

set( dbbgm_algo_sources
  dbbgm_hmm_algo.cxx         dbbgm_hmm_algo.h
)

aux_source_directory(Templates dbbgm_algo_sources)

add_library( dbbgm_algo  ${dbbgm_algo_sources} )

target_link_libraries( dbbgm_algo dbbgm vil)

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )



