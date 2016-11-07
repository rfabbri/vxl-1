# contrib/mleotta/vidreg/CMakeLists.txt
# feature based video registration

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )

set(vidreg_sources
  vidreg_detector.cxx             vidreg_detector.h
  vidreg_matcher.cxx              vidreg_matcher.h
  vidreg_feature_pt_desc.cxx      vidreg_feature_pt_desc.h
  vidreg_feature_edgel.cxx        vidreg_feature_edgel.h
  vidreg_initializer.cxx          vidreg_initializer.h
                                  vidreg_feature_group.h
  vidreg_registration.cxx         vidreg_registration.h
  vidreg_salient_group.cxx        vidreg_salient_group.h        vidreg_salient_group_sptr.h
  vidreg_tracker.cxx              vidreg_tracker.h
  vidreg_reconstructor.cxx        vidreg_reconstructor.h
)

aux_source_directory(Templates vidreg_sources)

add_library(vidreg  ${vidreg_sources})

target_link_libraries(vidreg rgrl vnl dbkpr gevd vil vpgl_algo)

if( BUILD_TESTING )
#  add_subdirectory(tests)
endif( BUILD_TESTING )


# algorithms 
#add_subdirectory( algo ) 

# processes
#add_subdirectory( pro )
