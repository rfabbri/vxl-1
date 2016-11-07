project(kdu_expand)
set(expand_SOURCES
expand_local.h
kdu_expand.cpp
../jp2/jp2.cpp
../args/args.cpp
main.cpp
)
set(background_exp_SOURCES
kdu_expand_bg.h
expand_local.h
kdu_expand.cpp
kdu_expand_bg.cpp
synthesis_bg.cpp
../jp2/jp2.cpp
../args/args.cpp

)
include_directories(../../coresys/common
../args
../image
../jp2
../compressed_io
)
add_executable(kdu_expand ${expand_SOURCES})
target_link_libraries(kdu_expand kdu_coresys kdu_image)
add_library(kdu_background_exp ${background_exp_SOURCES})
target_link_libraries( kdu_background_exp kdu_coresys kdu_image)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)