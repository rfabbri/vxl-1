# Brown edge tracking using particle library 
# brcv/trk/dbetrk/CMakeLists.txt
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

set(dbetrk_sources
  
   dbetrk_edge.cxx dbetrk_edge.h dbetrk_edge_sptr.h
   dbetrk_track.cxx dbetrk_track.h 
)
aux_source_directory(Templates dbetrk_sources)

add_library(dbetrk  ${dbetrk_sources})

target_link_libraries(dbetrk vbl vdgl vtol vsol vil vnl  btol mvl mbl vsl brip)

add_subdirectory(pro)
if (VXL_VGUI_FOUND)
add_subdirectory(vis)
endif (VXL_VGUI_FOUND)

if(BUILD_TESTING)
        add_subdirectory(tests)
endif(BUILD_TESTING)
