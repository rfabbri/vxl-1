
ADD_EXECUTABLE (recognizer  recognizer.cxx tube.h tube.cxx recogognizers_tree_type.h subsidary_routine.h)
TARGET_LINK_LIBRARIES(recognizer vgl vnl vul)



ADD_EXECUTABLE (recogtest  recogtest.cxx )
TARGET_LINK_LIBRARIES(recogtest vgl vnl vul)
