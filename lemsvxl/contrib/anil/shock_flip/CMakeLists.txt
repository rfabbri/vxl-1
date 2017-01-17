# This is contrib/anil/shock_flip/CMakeLists.txt
# Brown Shock Recognition Library Executables

set(hor_flip_shock_graph_sources
 hor_flip_shock_graph.h
 hor_flip_shock_graph.cxx
 test_main)

add_executable (hor_flip_shock_graph ${hor_flip_shock_graph_sources})
target_link_libraries (hor_flip_shock_graph dbskr dbsk2d vcl vsol)
