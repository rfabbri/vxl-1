#this is contrib/bm/bin/CMakeLists.txt

cmake_minimum_required(VERSION 2.6)

if(VXL_FOUND)

  add_executable( apply_mask_exec applyMask.cxx )

  target_link_libraries( apply_mask_exec vnl vil vul )

  add_executable( create_bw_images create_bw_images.cxx )

  target_link_libraries( create_bw_images vnl vil vul )

  add_executable( load_img load_img.cxx )
  
  target_link_libraries( load_img vil )

endif(VXL_FOUND)
