include_directories( ${VXL_BRL_INCLUDE_DIR} )
if(OCTAVE_FOUND)
set(engn2560_egb_sources
     convolve.h disjoint-set.h filter.h image.h imconv.h imutil.h misc.h pnmfile.h segment-graph.h segment-image.h egb-main.cxx  
    ) 
    
add_executable(engn2560_egb ${engn2560_egb_sources})
target_link_libraries(engn2560_egb vpl dbul dbxml)
endif(OCTAVE_FOUND)
