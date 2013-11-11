# playland/sadal/gui/CMakeLists.txt



IF (VXL_VGUI_FOUND)


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/blem/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/blem/breye )

SET(sadal_gui_sources

   sadal_menu.cxx                        sadal_menu.h
   sadal_main.cxx
   vidpro_plane_reconst_process.cxx      vidpro_plane_reconst_process.h
   vidpro_new_vsol2d_tablo_process.cxx   vidpro_new_vsol2d_tablo_process.h
   vidpro_homog_view_process.cxx         vidpro_homog_view_process.h  
   vidpro_combine_storage_process.cxx    vidpro_combine_storage_process.h  
   vidpro_factorize_process.cxx          vidpro_factorize_process.h 
   vidpro_kltrack_process.cxx            vidpro_kltrack_process.h 
   vidpro_affine_homog_est_process.cxx   vidpro_affine_homog_est_process.h
)

AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(sadal_gui ${sadal_gui_sources})

TARGET_LINK_LIBRARIES(sadal_gui dbvis bvis vidpro vidl rrel bprt vgel bfac vgl vgl_algo vsl brct mbl bgui3d bmrf)

ENDIF (VXL_VGUI_FOUND)
