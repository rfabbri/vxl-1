# This is brcv/seg/dbbgm/CMakeLists.txt
# Brown background modeling Library


set(dbbgm_sources
  dbbgm_distribution_image.txx    dbbgm_distribution_image.h    
  dbbgm_distribution_image_sptr.h dbbgm_distribution_image.cxx
  dbbgm_update.h
  dbbgm_apply.h
  dbbgm_detect.h

  dbbgm_bgmodel.h       dbbgm_bgmodel.txx dbbgm_bgmodel_sptr.h
  dbbgm_model_data.h    dbbgm_model_data.cxx


)

aux_source_directory(Templates dbbgm_sources)

add_library(dbbgm  ${dbbgm_sources})

# add the required libraries into this list
target_link_libraries(dbbgm dbsta vsl vbl vbl_io)  


if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )


# algorithms for background modeling
add_subdirectory( algo )

# process for background modeling
add_subdirectory(pro)

# visualization for background modeling
add_subdirectory(vis)



