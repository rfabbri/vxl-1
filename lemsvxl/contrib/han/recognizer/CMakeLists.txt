
add_executable (recognizer  recognizer.cxx tube.h tube.cxx recogognizers_tree_type.h subsidary_routine.h)
target_link_libraries(recognizer vgl vnl vul)



add_executable (recogtest  recogtest.cxx )
target_link_libraries(recogtest vgl vnl vul)
