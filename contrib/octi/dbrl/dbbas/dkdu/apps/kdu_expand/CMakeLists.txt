PROJECT(kdu_expand)
SET(expand_SOURCES
expand_local.h
kdu_expand.cpp
../jp2/jp2.cpp
../args/args.cpp
main.cpp
)
SET(background_exp_SOURCES
kdu_expand_bg.h
expand_local.h
kdu_expand.cpp
kdu_expand_bg.cpp
synthesis_bg.cpp
../jp2/jp2.cpp
../args/args.cpp

)
INCLUDE_DIRECTORIES(../../coresys/common
../args
../image
../jp2
../compressed_io
)
ADD_EXECUTABLE(kdu_expand ${expand_SOURCES})
TARGET_LINK_LIBRARIES(kdu_expand kdu_coresys kdu_image)
ADD_LIBRARY(kdu_background_exp ${background_exp_SOURCES})
TARGET_LINK_LIBRARIES( kdu_background_exp kdu_coresys kdu_image)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)