#brl/J2KTest
SET(entCoding1_sources
init_resource.h
init_resource.cpp
main.cxx)

ADD_EXECUTABLE(entCoding1 ${entCoding1_sources})
TARGET_LINK_LIBRARIES(entCoding1 vil vul NCSEcw) 