SET(tube_source
    tube.cxx tube.h
    )

ADD_LIBRARY(dtube_rec ${tube_source})

#TARGET_LINK_LIBRARIES(dtube)


ADD_EXECUTABLE (dtube_test_rec  main.cxx)
TARGET_LINK_LIBRARIES(dtube_test_rec dtube_rec vgl vnl vul)

ADD_EXECUTABLE (dtube_learn_rec  main_learn.cxx control.cxx recog.h main.h sub.h)
TARGET_LINK_LIBRARIES(dtube_learn_rec dtube_rec vgl vnl vul)


ADD_EXECUTABLE (learn_rec  learn.cxx )
TARGET_LINK_LIBRARIES(learn_rec dtube_rec vgl vnl vul)



