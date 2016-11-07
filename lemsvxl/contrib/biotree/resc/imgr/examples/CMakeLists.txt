# blem/contrib/biotree/resc/imgr/examples/CMakeLists.txt

#include_directories( ${VXL_MUL_INCLUDE_DIR})
#include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )


add_executable(imgr_crop_scan_images_example imgr_crop_scan_images_example.cxx )
target_link_libraries(imgr_crop_scan_images_example dbil vgl vil vsl)
#target_link_libraries(imgr_crop_scan_images_example imgr imgr_file_format dbil vgl vil vsl)


  
