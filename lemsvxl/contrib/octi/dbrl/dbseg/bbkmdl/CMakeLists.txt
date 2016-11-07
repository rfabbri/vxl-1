set(bbkmdl2910x_sources 

init_resource.h
init_resource.cpp
main.cxx)

add_executable(bbkmdl2910x ${bbkmdl2910x_sources})
target_link_libraries(bbkmdl2910x vil vidl vul bbgm vcl brdb bprb bbgm_pro)


