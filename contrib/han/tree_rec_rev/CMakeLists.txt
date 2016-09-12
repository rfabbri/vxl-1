set(tube_source
    tube_rev.cxx tube_rev.h
    )

add_library(dtube_rec_rev ${tube_source})

#target_link_libraries(dtube)


add_executable (dtube_test_rec_rev  main_rev.cxx)
target_link_libraries(dtube_test_rec_rev dtube_rec_rev vgl vnl vul)

add_executable (dtube_learn_rec_rev  main_learn_rev.cxx control_rev.cxx recog_rev.h main_rev.h sub_rev.h)
target_link_libraries(dtube_learn_rec_rev dtube_rec_rev vgl vnl vul)


add_executable (learn_rec_rev  learn_rev.cxx )
target_link_libraries(learn_rec_rev dtube_rec_rev vgl vnl vul)



