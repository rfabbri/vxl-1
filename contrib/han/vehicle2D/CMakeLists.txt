
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR}/mbl )
#add_executable (CRHM  main.cxx )
#target_link_libraries(CRHM vgl vnl vul mbl)


set(vehicle2D_source
    vehicle2D.cxx vehicle2D.h
    )

add_library(vehicle2D ${vehicle2D_source})
target_link_libraries(vehicle2D vgl vgl_algo  vnl vul mbl )





#target_link_libraries(dtube)
# Processing Library
#add_subdirectory(exec)

#add_executable (dtube_test  main.cxx)
#target_link_libraries(dtube_test dtube vgl vnl vul)

#add_executable (dtube_learn  main_learn.cxx control.cxx recog.h main.h sub.h)
#target_link_libraries(dtube_learn dtube vgl vnl vul)


add_executable (test_vehicle2D   test_vehicle2D.cxx  ps_xz.h)
target_link_libraries(test_vehicle2D vehicle2D vgl vnl vul)



