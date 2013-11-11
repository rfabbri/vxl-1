#this is contrib/bm/bin/CMakeLists.txt

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(VXL_FOUND)

  ADD_EXECUTABLE( apply_mask_exec applyMask.cxx )

  TARGET_LINK_LIBRARIES( apply_mask_exec vnl vil vul )

  ADD_EXECUTABLE( create_bw_images create_bw_images.cxx )

  TARGET_LINK_LIBRARIES( create_bw_images vnl vil vul )

  ADD_EXECUTABLE( load_img load_img.cxx )
  
  TARGET_LINK_LIBRARIES( load_img vil )

ENDIF(VXL_FOUND)
