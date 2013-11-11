# brcv/mvg/dbvrl/CMakeLists.txt
# Brown video registration library

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(dbvrl_sources
   dbvrl_world_roi.cxx         dbvrl_world_roi.h          dbvrl_world_roi_sptr.h  
   dbvrl_region.cxx            dbvrl_region.h             dbvrl_region_sptr.h   
                               dbvrl_transform_2d.h       dbvrl_transform_2d_sptr.h     
   dbvrl_cost_func.cxx         dbvrl_cost_func.h  
   dbvrl_minimizer.cxx         dbvrl_minimizer.h 
   dbvrl_lsqr_cost_func.cxx    dbvrl_lsqr_cost_func.h 

   dbvrl_super_res.cxx         dbvrl_super_res.h
)

AUX_SOURCE_DIRECTORY(Templates dbvrl_sources)

ADD_LIBRARY(dbvrl  ${dbvrl_sources})

TARGET_LINK_LIBRARIES(dbvrl vnl vil vimt vbl vsl)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

# Executable library
SUBDIRS(exe)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

