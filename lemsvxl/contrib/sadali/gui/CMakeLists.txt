# playland/sadal/gui/CMakeLists.txt



if (VXL_VGUI_FOUND)


include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${RPL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_RPL_INCLUDE_DIR}/rrel )
include_directories( ${LEMS_SOURCE_DIR}/contrib/sadali/dbbbopt_vis )

include_directories( ${LEMS_SOURCE_DIR}/contrib/sadali/dbbbopt_pro )

set(sadal_gui_sources

   sadali_menu.cxx                              sadali_menu.h
   sadali_main.cxx
 #  vidpro1_plane_reconst_process.cxx            vidpro1_plane_reconst_process.h
 #  vidpro1_plane_refine_process.cxx             vidpro1_plane_refine_process.h
   vidpro1_edgemap_form_process.cxx              vidpro1_edgemap_form_process.h
 #  vidpro1_edgemap_form_dc_process.cxx           vidpro1_edgemap_form_dc_process.h
   vidpro1_plane_opt_process.cxx                 vidpro1_plane_opt_process.h
   vidpro1_new_vsol2d_tablo_process.cxx          vidpro1_new_vsol2d_tablo_process.h
   vidpro1_homog_view_process.cxx                vidpro1_homog_view_process.h  
   vidpro1_combine_storage_process.cxx           vidpro1_combine_storage_process.h  
   vidpro1_factorize_process.cxx                 vidpro1_factorize_process.h 
   vidpro1_kltrack_process.cxx                   vidpro1_kltrack_process.h 
   vidpro1_affine_homog_est_process.cxx          vidpro1_affine_homog_est_process.h
   vidpro1_BB_opt_test_process.cxx               vidpro1_BB_opt_test_process.h
   vidpro1_BB_visualize_process.cxx              vidpro1_BB_visualize_process.h
   BB_optimization_problem.cxx                  BB_optimization_problem.h
   vidpro1_3D_planar_curve_reconst_process.cxx   vidpro1_3D_planar_curve_reconst_process.h
   vidpro1_homog_curve_process.cxx               vidpro1_homog_curve_process.h
   planar_curve_reconst_problem.cxx             planar_curve_reconst_problem.h
)

aux_source_directory(Templates bvis1_sources)

add_executable(sadal_gui ${sadal_gui_sources})

target_link_libraries(sadal_gui  bvis1 vidpro1 vidl1 rrel bprt vgel bfac vgl vgl_algo vsl brct  dbbbopt_vis dbhom_vis)

endif (VXL_VGUI_FOUND)
