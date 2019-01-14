include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories(${LEMS_SOURCE_DIR})
include_directories(${LEMS_SOURCE_DIR}/basic)

#added for livewire:
#include_directories( ${VXL_OXL_INCLUDE_DIR} )


if (VXL_VGUI_FOUND)


set(HindSiteCombinedExecutable_sources

                                                bfrag_Main.cxx
    bfrag_Main_Menu.h                           bfrag_Main_Menu.cxx
    bfrag_Manager.h                             bfrag_Manager.cxx
    bfrag_Menu.h                                bfrag_Menu.cxx
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
#    Point.h
#    Curve.h                                     
    DPMatch.h
    FineDPMatch.h
    params.h
    extern_params.h
#    global.h
    utils.h

#sptr
    vgui_bfrag2D_tableau_sptr.h

#livewire
#   dbvis1_livewire_tools.cxx                   dbvis1_livewire_tools.h

# additions by Can
bfrag_curve.h   
bfrag_curve.cxx 
read_write.cxx
)

add_executable(HindSiteCombinedExecutable ${HindSiteCombinedExecutable_sources})

target_link_libraries(HindSiteCombinedExecutable vsol bsol bgui vil vcl dbsol dbsol_algo bxml) 
#add for liveWire: dbvis


endif (VXL_VGUI_FOUND)

set(HindSiteCombinedExecutable_lib_sources
bfrag2D.cxx
# direct from jonah
PuzzleSolving.cxx
# additions by Can
bfrag_curve.cxx
read_write.cxx
)

add_library(HindSiteCombinedExecutable_lib  ${HindSiteCombinedExecutable_lib_sources})
target_link_libraries(HindSiteCombinedExecutable_lib vsol bsol bgui vil vcl dbsol dbsol_algo) 

add_subdirectory(web_system_executables)

set(HindSiteCombinedExecutable_vis_sources

                                                my_bfrag_Main.cxx
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
    params.h
    extern_params.h
    utils.h

#sptr
    vgui_bfrag2D_tableau_sptr.h

# additions by Can
bfrag_curve.h   
bfrag_curve.cxx 
read_write.cxx
)

add_executable(HindSiteCombinedExecutable_vis ${HindSiteCombinedExecutable_vis_sources})

target_link_libraries(HindSiteCombinedExecutable_vis vsol bsol bgui vil vcl dbsol dbsol_algo bxml) 


