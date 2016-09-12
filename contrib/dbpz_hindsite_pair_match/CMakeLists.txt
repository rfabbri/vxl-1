# breyes/bfrag/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

if (VXL_VGUI_FOUND)


set(pairMatch_sources

  main.cpp
  pairMatcher.h                pairMatcher.cpp
    my_bsol_intrinsic_curve_2d.h    my_bsol_intrinsic_curve_2d.cpp
    pairMatchMainMenu.h             pairMatchMainMenu.cpp
    pairMatchMenu.h                 pairMatchMenu.cpp
    pairMatchManager.h              pairMatchManager.cpp

  #curve matching
  curve_dpmatch_2d.h              curve_dpmatch_2d.cpp

)

add_executable(pairMatch ${pairMatch_sources})

target_link_libraries(pairMatch vsol bsol bgui vcl) 
#target_link_libraries(pairMatch vsol bsol bgui vil vcl) 


endif (VXL_VGUI_FOUND)
