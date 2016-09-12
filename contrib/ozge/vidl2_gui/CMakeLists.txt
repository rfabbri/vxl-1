# brl/vvid/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg)
include_directories( ${lems_SOURCE_DIR}/contrib )

# determines if vgui is built and opengl is available

if(VXL_VGUI_FOUND)

 #find the path to directshow
# include( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )

 if(DIRECTSHOW_FOUND)
  include_directories(${DIRECTSHOW_INCLUDE_DIR})
#  add_definitions(-DHAS_DSHOW)
  set(directshow_sources
      vvid_live_video_dshow_tableau.cxx  vvid_live_video_dshow_tableau.h
                                         vvid_live_video_dshow_tableau_sptr.h
      vvid_live_video_dshow_manager.cxx  vvid_live_video_dshow_manager.h
  )
 endif(DIRECTSHOW_FOUND)

  if(DIRECTSHOW_FOUND)
    add_library(ozgevvid ${directshow_sources})
    target_link_libraries(ozgevvid vpro strk sdet btol bsol vtol vsol brip bgui vgui vidl vidl_vil1 vul vil vil1 vidl)
  endif(DIRECTSHOW_FOUND)

# install_targets(/lib vvid)
# INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/vvid ${vvid_sources})

 

 # the main executables
 subdirs(bin)

endif(VXL_VGUI_FOUND)
