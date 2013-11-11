# This is brcv/seg/dbbgm/CMakeLists.txt
# Brown background modeling Library


SET(dbbgm_sources
  dbbgm_distribution_image.txx    dbbgm_distribution_image.h    
  dbbgm_distribution_image_sptr.h dbbgm_distribution_image.cxx
  dbbgm_update.h
  dbbgm_apply.h
  dbbgm_detect.h

  dbbgm_bgmodel.h       dbbgm_bgmodel.txx dbbgm_bgmodel_sptr.h
  dbbgm_model_data.h    dbbgm_model_data.cxx


)

AUX_SOURCE_DIRECTORY(Templates dbbgm_sources)

ADD_LIBRARY(dbbgm  ${dbbgm_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES(dbbgm dbsta vsl vbl vbl_io)  


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )


# algorithms for background modeling
SUBDIRS( algo )

# process for background modeling
SUBDIRS(pro)

# visualization for background modeling
SUBDIRS(vis)



