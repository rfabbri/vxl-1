include_directories( ${lems_SOURCE_DIR}/basic)
include_directories( ${lems_SOURCE_DIR}/contrib/firat/HindSiteAnant)
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories(${lems_SOURCE_DIR})

set(HindSiteCombinedExecutable_sources2

                                                #my_bfrag_Main.cxx
    my_bfrag_Main_Menu.h                        my_bfrag_Main_Menu.cxx
    bfrag_Manager.h                             bfrag_Manager.cxx
    my_bfrag_Menu.h                             my_bfrag_Menu.cxx
    bfrag2DManager.h                            bfrag2DManager.cxx
    bfrag2D.h                                   bfrag2D.cxx
    vgui_soview2D_bfrag2D.h                     vgui_soview2D_bfrag2D.cxx
    vgui_bfrag2D_tableau.h                      vgui_bfrag2D_tableau.cxx
    vgui_soview2D_bfrag2D_assembly.h            vgui_soview2D_bfrag2D_assembly.cxx
    bfrag2D_assembly.h                          bfrag2D_assembly.cxx
    bgui_vsol_soview2D_new.h                    bgui_vsol_soview2D_new.cxx

#direct from jonah
    PuzzleSolving.h                             PuzzleSolving.cxx
    xform.h                                   
    DPMatch.h
    FineDPMatch.h
   # params.h
    extern_params.h
    utils.h

#sptr
    vgui_bfrag2D_tableau_sptr.h

# additions by Can
bfrag_curve.h   
bfrag_curve.cxx 
read_write.cxx
#Anant
dbpz_hindsite_web_combined_process.cxx
)
add_executable(HindSiteCombinedExecutable_combined_process ${HindSiteCombinedExecutable_sources2})
target_link_libraries(HindSiteCombinedExecutable_combined_process bxml HindSiteCombinedExecutable_lib vsol bsol bgui vil vcl dbsol dbsol_algo bxml)
