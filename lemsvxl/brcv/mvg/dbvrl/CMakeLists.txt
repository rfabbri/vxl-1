# brcv/mvg/dbvrl/CMakeLists.txt
# Brown video registration library

include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(dbvrl_sources
   dbvrl_world_roi.cxx         dbvrl_world_roi.h          dbvrl_world_roi_sptr.h  
   dbvrl_region.cxx            dbvrl_region.h             dbvrl_region_sptr.h   
                               dbvrl_transform_2d.h       dbvrl_transform_2d_sptr.h     
   dbvrl_cost_func.cxx         dbvrl_cost_func.h  
   dbvrl_minimizer.cxx         dbvrl_minimizer.h 
   dbvrl_lsqr_cost_func.cxx    dbvrl_lsqr_cost_func.h 

   dbvrl_super_res.cxx         dbvrl_super_res.h
)

aux_source_directory(Templates dbvrl_sources)

add_library(dbvrl  ${dbvrl_sources})

target_link_libraries(dbvrl vnl vil vimt vbl vsl)

if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

# Executable library
add_subdirectory(exe)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

