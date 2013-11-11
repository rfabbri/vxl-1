# Logical Linear library
# algo/loglin/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )


SET(loglin_sources

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
AUX_SOURCE_DIRECTORY(Templates loglin_sources)

#ADD_LIBRARY(loglin  ${loglin_sources})

# Currently nothing to compile.  All files are header files.
# Only inline functions and template definitions are here
#TARGET_LINK_LIBRARIES(loglin)

