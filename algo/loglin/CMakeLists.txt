# Logical Linear library
# algo/loglin/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )


set(loglin_sources

EdgeDetection.h
EdgeDetector.h
erfinv.h
LogicalLinearFilters.h
LogicalLinearFunctions.h
LogicalLinear.h
LogicalLinearPixel.h
points.h
PowellsAlgorithm.h
SubPixel.h
SubPixelOptimizer.h
Tokenizer.h

)
aux_source_directory(Templates loglin_sources)

#add_library(loglin  ${loglin_sources})

# Currently nothing to compile.  All files are header files.
# Only inline functions and template definitions are here
#target_link_libraries(loglin)

