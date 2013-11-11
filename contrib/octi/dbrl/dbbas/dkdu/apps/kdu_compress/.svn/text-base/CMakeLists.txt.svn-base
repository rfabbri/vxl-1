project(kdu_compress)
SET(compress_SOURCES
compress_local.h
kdu_compress.cpp
roi_sources.cpp
roi_sources.h
../jp2/jp2.cpp
../args/args.cpp
main.cpp
)

SET(background_SOURCES
compress_local.h
kdu_compress.cpp
roi_sources.cpp
analysis_bg.cpp
kdu_compress_bg.cpp
roi_sources.h
../jp2/jp2.cpp
../args/args.cpp
)
INCLUDE_DIRECTORIES(../../coresys/common
../args
../image
../compressed_io
../jp2
)
ADD_LIBRARY(kdu_background_comp ${background_SOURCES})
ADD_EXECUTABLE(kdu_compress ${compress_SOURCES})
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
TARGET_LINK_LIBRARIES(kdu_compress kdu_coresys kdu_image)
TARGET_LINK_LIBRARIES(kdu_background_comp kdu_coresys kdu_image)
