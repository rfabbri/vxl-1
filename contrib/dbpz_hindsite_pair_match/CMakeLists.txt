# breyes/bfrag/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

IF (VXL_VGUI_FOUND)


SET(pairMatch_sources

  main.cpp
  pairMatcher.h                pairMatcher.cpp
    my_bsol_intrinsic_curve_2d.h    my_bsol_intrinsic_curve_2d.cpp
    pairMatchMainMenu.h             pairMatchMainMenu.cpp
    pairMatchMenu.h                 pairMatchMenu.cpp
    pairMatchManager.h              pairMatchManager.cpp

  #curve matching
  curve_dpmatch_2d.h              curve_dpmatch_2d.cpp

)

ADD_EXECUTABLE(pairMatch ${pairMatch_sources})

TARGET_LINK_LIBRARIES(pairMatch vsol bsol bgui vcl) 
#TARGET_LINK_LIBRARIES(pairMatch vsol bsol bgui vil vcl) 


ENDIF (VXL_VGUI_FOUND)
