if (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/contrib/tpollard )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )


set(tpollard_gui_sources

   tpollard_menu.cxx       tpollard_menu.h
   tpollard_main.cxx
)


add_executable( tpollard_gui ${tpollard_gui_sources})

target_link_libraries(tpollard_gui bvis1 dbccl_pro )

endif (VXL_VGUI_FOUND)
