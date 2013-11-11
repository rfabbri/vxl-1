# contrib/mleotta/vidreg/CMakeLists.txt
# feature based video registration

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )

SET(vidreg_sources
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

AUX_SOURCE_DIRECTORY(Templates vidreg_sources)

ADD_LIBRARY(vidreg  ${vidreg_sources})

TARGET_LINK_LIBRARIES(vidreg rgrl vnl dbkpr gevd vil vpgl_algo)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )


# algorithms 
#SUBDIRS( algo ) 

# processes
#SUBDIRS( pro )
