
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR}/mbl )
#add_executable (CRHM  main.cxx )
#target_link_libraries(CRHM vgl vnl vul mbl)


set(vehicle3D_source
    vehicle3D.cxx vehicle3D.h
    )

add_library(vehicle3D ${vehicle3D_source})
target_link_libraries(vehicle3D vgl vgl_algo  vnl vul mbl )





#target_link_libraries(dtube)
# Processing Library
#subdirs(exec)

#add_executable (dtube_test  main.cxx)
#target_link_libraries(dtube_test dtube vgl vnl vul)

#add_executable (dtube_learn  main_learn.cxx control.cxx recog.h main.h sub.h)
#target_link_libraries(dtube_learn dtube vgl vnl vul)


add_executable (test_vehicle3D   test_vehicle3D.cxx )
target_link_libraries(test_vehicle3D vehicle3D vgl vnl vul)



