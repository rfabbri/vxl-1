
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR}/mbl )
#ADD_EXECUTABLE (CRHM  main.cxx )
#TARGET_LINK_LIBRARIES(CRHM vgl vnl vul mbl)


SET(vehicle2D_source
    vehicle2D.cxx vehicle2D.h
    )

ADD_LIBRARY(vehicle2D ${vehicle2D_source})
TARGET_LINK_LIBRARIES(vehicle2D vgl vgl_algo  vnl vul mbl )





#TARGET_LINK_LIBRARIES(dtube)
# Processing Library
#SUBDIRS(exec)

#ADD_EXECUTABLE (dtube_test  main.cxx)
#TARGET_LINK_LIBRARIES(dtube_test dtube vgl vnl vul)

#ADD_EXECUTABLE (dtube_learn  main_learn.cxx control.cxx recog.h main.h sub.h)
#TARGET_LINK_LIBRARIES(dtube_learn dtube vgl vnl vul)


ADD_EXECUTABLE (test_vehicle2D   test_vehicle2D.cxx  ps_xz.h)
TARGET_LINK_LIBRARIES(test_vehicle2D vehicle2D vgl vnl vul)



