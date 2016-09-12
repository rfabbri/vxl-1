# playland/sadal/gui/CMakeLists.txt



if (VXL_VGUI_FOUND)


include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${RPL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${VXL_BRL_INCLUDE_DIR}/blem/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/blem/breye )

set(sadal_gui_sources

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

aux_source_directory(Templates bvis_sources)

add_executable(sadal_gui ${sadal_gui_sources})

target_link_libraries(sadal_gui dbvis bvis vidpro vidl rrel bprt vgel bfac vgl vgl_algo vsl brct mbl bgui3d bmrf)

endif (VXL_VGUI_FOUND)
