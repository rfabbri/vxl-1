project(kdu_image)
include_directories(../../coresys/common)
set(SOURCES
image_local.h
kdu_image.h
image_in.cpp
image_out.cpp
palette.cpp
)
add_library(kdu_image STATIC ${SOURCES})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
