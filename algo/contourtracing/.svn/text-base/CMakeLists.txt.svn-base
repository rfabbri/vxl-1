# contourtracing library
# algo/contourtracing/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(contourtracing_sources

1d_eno_types.h           
basic_utils.cpp          
boundary_fill.cpp        
compute-boundary-len.cpp 
ContourTracing.cpp       
contour_processing.cpp               
contour_tracer.cpp                   
curvature_smoothing.cpp          
direction.cpp                    
eno_image.cpp                    
eno_interpolant.cpp              
eno_interval.cpp                 
eno_label_zerox.cpp              
eno_label_zerox.h                
eno_shock.cpp                        
eno_utils.cpp                        
eno_zero_crossing.cpp                
eno_zero_tracer.cpp                  
euclidean_dist.cpp                   
euclidean_dist.h         
graphics.h               
image_processing.h
next_xing_location.cpp                    
pgm.cpp                  
pgm.h                    
redt.h                   
signed_dt.cpp            
signed_dt.h              
twod_dpoint.h            
utils.cpp                
utils.h                  
zero_tracer.cpp          
ContourTracing.h
ContourTracing_sptr.h

)

AUX_SOURCE_DIRECTORY(Templates contourtracing_sources)

ADD_LIBRARY(contourtracing  ${contourtracing_sources})

TARGET_LINK_LIBRARIES(contourtracing  vgl vcl vnl vbl vsol )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )



