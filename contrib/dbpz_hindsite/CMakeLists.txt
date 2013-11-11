# lemsvxlsrc/contrib/dbpz_hindsite/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

#added for livewire:
#INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )


IF (VXL_VGUI_FOUND)


SET(dbpz_hindsite_sources

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

ADD_EXECUTABLE(dbpz_hindsite ${dbpz_hindsite_sources})

TARGET_LINK_LIBRARIES(dbpz_hindsite vsol bsol bgui vil vcl) 
#TARGET_LINK_LIBRARIES(dbpz_hindsite vsol bsol bgui dbvis1) 
#add for liveWire: dbvis1


ENDIF (VXL_VGUI_FOUND)
