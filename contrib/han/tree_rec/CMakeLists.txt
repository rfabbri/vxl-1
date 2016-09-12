set(tube_source
    tube.cxx tube.h
    )

add_library(dtube_rec ${tube_source})

#target_link_libraries(dtube)


add_executable (dtube_test_rec  main.cxx)
target_link_libraries(dtube_test_rec dtube_rec vgl vnl vul)

add_executable (dtube_learn_rec  main_learn.cxx control.cxx recog.h main.h sub.h)
target_link_libraries(dtube_learn_rec dtube_rec vgl vnl vul)


add_executable (learn_rec  learn.cxx )
target_link_libraries(learn_rec dtube_rec vgl vnl vul)



