project(kdu_compress)
set(compress_SOURCES
compress_local.h
kdu_compress.cpp
roi_sources.cpp
roi_sources.h
../jp2/jp2.cpp
../args/args.cpp
main.cpp
)

set(background_SOURCES
compress_local.h
kdu_compress.cpp
roi_sources.cpp
analysis_bg.cpp
kdu_compress_bg.cpp
roi_sources.h
../jp2/jp2.cpp
../args/args.cpp
)
include_directories(../../coresys/common
../args
../image
../compressed_io
../jp2
)
add_library(kdu_background_comp ${background_SOURCES})
add_executable(kdu_compress ${compress_SOURCES})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
target_link_libraries(kdu_compress kdu_coresys kdu_image)
target_link_libraries(kdu_background_comp kdu_coresys kdu_image)
