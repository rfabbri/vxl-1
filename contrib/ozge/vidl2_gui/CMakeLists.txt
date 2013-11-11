# brl/vvid/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

# determines if vgui is built and opengl is available

IF(VXL_VGUI_FOUND)

 #find the path to directshow
# INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )

 IF(DIRECTSHOW_FOUND)
  INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIR})
#  ADD_DEFINITIONS(-DHAS_DSHOW)
  SET(directshow_sources
      vvid_live_video_dshow_tableau.cxx  vvid_live_video_dshow_tableau.h
                                         vvid_live_video_dshow_tableau_sptr.h
      vvid_live_video_dshow_manager.cxx  vvid_live_video_dshow_manager.h
  )
 ENDIF(DIRECTSHOW_FOUND)

  IF(DIRECTSHOW_FOUND)
    ADD_LIBRARY(ozgevvid ${directshow_sources})
    TARGET_LINK_LIBRARIES(ozgevvid vpro strk sdet btol bsol vtol vsol brip bgui vgui vidl vidl_vil1 vul vil vil1 vidl)
  ENDIF(DIRECTSHOW_FOUND)

# INSTALL_TARGETS(/lib vvid)
# INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/vvid ${vvid_sources})

 

 # the main executables
 SUBDIRS(bin)

ENDIF(VXL_VGUI_FOUND)
