# This is contrib/anil/shock_flip/CMakeLists.txt
# Brown Shock Recognition Library Executables

SET(hor_flip_shock_graph_sources
 hor_flip_shock_graph.h
 hor_flip_shock_graph.cxx
 test_main)

ADD_EXECUTABLE (hor_flip_shock_graph ${hor_flip_shock_graph_sources})
TARGET_LINK_LIBRARIES (hor_flip_shock_graph dbskr dbsk2d vcl vsol)
