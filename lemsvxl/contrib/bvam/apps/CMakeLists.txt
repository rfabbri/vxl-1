# This is contrib/bvam/exe/CMakeLists.txt
#

add_executable( bvam_main bvam_main.cxx )
target_link_libraries( bvam_main bvam )

#add_executable( bwm_img_main bwm_img_main.cxx )
#target_link_libraries( bwm_img_main bwm vgui vil )
