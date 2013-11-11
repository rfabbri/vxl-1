SET(tube_source
    tube_rev.cxx tube_rev.h
    )

ADD_LIBRARY(dtube_rec_rev ${tube_source})

#TARGET_LINK_LIBRARIES(dtube)


ADD_EXECUTABLE (dtube_test_rec_rev  main_rev.cxx)
TARGET_LINK_LIBRARIES(dtube_test_rec_rev dtube_rec_rev vgl vnl vul)

ADD_EXECUTABLE (dtube_learn_rec_rev  main_learn_rev.cxx control_rev.cxx recog_rev.h main_rev.h sub_rev.h)
TARGET_LINK_LIBRARIES(dtube_learn_rec_rev dtube_rec_rev vgl vnl vul)


ADD_EXECUTABLE (learn_rec_rev  learn_rev.cxx )
TARGET_LINK_LIBRARIES(learn_rec_rev dtube_rec_rev vgl vnl vul)



