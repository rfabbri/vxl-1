# lemsvxlsrc/contrib/dbpz_hindsite/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

#added for livewire:
#include_directories( ${VXL_OXL_INCLUDE_DIR} )


if (VXL_VGUI_FOUND)


set(dbpz_hindsite_sources

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
    Point.h
    Curve.h                                     
    DPMatch.h
    FineDPMatch.h
    global.h
    utils.h

#sptr
    vgui_bfrag2D_tableau_sptr.h

#livewire
#   dbvis1_livewire_tools.cxx                   dbvis1_livewire_tools.h

)

add_executable(dbpz_hindsite ${dbpz_hindsite_sources})

target_link_libraries(dbpz_hindsite vsol bsol bgui vil vcl) 
#target_link_libraries(dbpz_hindsite vsol bsol bgui dbvis1) 
#add for liveWire: dbvis1


endif (VXL_VGUI_FOUND)
