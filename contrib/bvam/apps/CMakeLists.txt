# This is contrib/bvam/exe/CMakeLists.txt
#

ADD_EXECUTABLE( bvam_main bvam_main.cxx )
TARGET_LINK_LIBRARIES( bvam_main bvam )

#ADD_EXECUTABLE( bwm_img_main bwm_img_main.cxx )
#TARGET_LINK_LIBRARIES( bwm_img_main bwm vgui vil )
