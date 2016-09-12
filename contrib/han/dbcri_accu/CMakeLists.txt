set(tube_source
    tube.cxx tube.h
    )

add_library(dtube ${tube_source})

#target_link_libraries(dtube)
# Processing Library
subdirs(pro)

add_executable (dtube_test  main.cxx)
target_link_libraries(dtube_test dtube vgl vnl vul)

add_executable (dtube_learn  main_learn.cxx control.cxx recog.h main.h sub.h)
target_link_libraries(dtube_learn dtube vgl vnl vul)


add_executable (learn  learn.cxx )
target_link_libraries(learn dtube vgl vnl vul)



