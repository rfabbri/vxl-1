#brl/J2KTest
set(entCoding1_sources
init_resource.h
init_resource.cpp
main.cxx)

add_executable(entCoding1 ${entCoding1_sources})
target_link_libraries(entCoding1 vil vul NCSEcw) 