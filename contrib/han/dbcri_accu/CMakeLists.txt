SET(tube_source
    tube.cxx tube.h
    )

ADD_LIBRARY(dtube ${tube_source})

#TARGET_LINK_LIBRARIES(dtube)
# Processing Library
SUBDIRS(pro)

ADD_EXECUTABLE (dtube_test  main.cxx)
TARGET_LINK_LIBRARIES(dtube_test dtube vgl vnl vul)

ADD_EXECUTABLE (dtube_learn  main_learn.cxx control.cxx recog.h main.h sub.h)
TARGET_LINK_LIBRARIES(dtube_learn dtube vgl vnl vul)


ADD_EXECUTABLE (learn  learn.cxx )
TARGET_LINK_LIBRARIES(learn dtube vgl vnl vul)



