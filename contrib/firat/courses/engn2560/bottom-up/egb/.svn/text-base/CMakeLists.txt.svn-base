INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
IF(OCTAVE_FOUND)
SET(engn2560_egb_sources
     convolve.h disjoint-set.h filter.h image.h imconv.h imutil.h misc.h pnmfile.h segment-graph.h segment-image.h egb-main.cxx  
    ) 
    
ADD_EXECUTABLE(engn2560_egb ${engn2560_egb_sources})
TARGET_LINK_LIBRARIES(engn2560_egb vpl dbul dbxml)
ENDIF(OCTAVE_FOUND)
