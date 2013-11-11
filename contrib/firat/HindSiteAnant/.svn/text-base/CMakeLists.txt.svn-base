INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR})
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)

#added for livewire:
#INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )


IF (VXL_VGUI_FOUND)


SET(HindSiteCombinedExecutable_sources

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

ADD_EXECUTABLE(HindSiteCombinedExecutable ${HindSiteCombinedExecutable_sources})

TARGET_LINK_LIBRARIES(HindSiteCombinedExecutable vsol bsol bgui vil vcl dbsol dbsol_algo bxml) 
#add for liveWire: dbvis


ENDIF (VXL_VGUI_FOUND)

SET(HindSiteCombinedExecutable_lib_sources
bfrag2D.cxx
# direct from jonah
PuzzleSolving.cxx
# additions by Can
bfrag_curve.cxx
read_write.cxx
)

ADD_LIBRARY(HindSiteCombinedExecutable_lib  ${HindSiteCombinedExecutable_lib_sources})
TARGET_LINK_LIBRARIES(HindSiteCombinedExecutable_lib vsol bsol bgui vil vcl dbsol dbsol_algo) 

SUBDIRS(web_system_executables)

SET(HindSiteCombinedExecutable_vis_sources

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

ADD_EXECUTABLE(HindSiteCombinedExecutable_vis ${HindSiteCombinedExecutable_vis_sources})

TARGET_LINK_LIBRARIES(HindSiteCombinedExecutable_vis vsol bsol bgui vil vcl dbsol dbsol_algo bxml) 


