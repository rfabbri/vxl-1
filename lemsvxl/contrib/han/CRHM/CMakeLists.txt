include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR}/mbl )
add_executable (CRHM  main.cxx )
target_link_libraries(CRHM vgl vnl vul mbl)



