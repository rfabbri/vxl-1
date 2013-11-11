# lemsvxlsrc/basic/dbgl/CMakeLists.txt
# The Development Brown Geometry Library

SET( dbgl_sources

   dbgl_fuzzy_boolean.h
   dbgl_dist.h                dbgl_dist.cxx
   dbgl_triangle.h            dbgl_triangle.cxx
   dbgl_barycentric.h         dbgl_barycentric.cxx

   dbgl_param_curve.h         dbgl_param_curve.cxx
   dbgl_poly_curve.h          dbgl_poly_curve.cxx     
   dbgl_poly_curve_line.h     dbgl_poly_curve_line.cxx
   dbgl_eulerspiral_base.h    dbgl_eulerspiral_base.cxx
   dbgl_arc_base.h            dbgl_arc_base.cxx
   dbgl_arc.h                 dbgl_arc.cxx
   dbgl_eno_curve.h           dbgl_eno_curve.cxx
   dbgl_minimal_container.h   dbgl_minimal_container.txx

   dbgl_param_curve_3d.h      dbgl_param_curve_3d.cxx
   dbgl_eno_curve_3d.h        dbgl_eno_curve_3d.cxx
   dbgl_linear_curve_3d.h     dbgl_linear_curve_3d.cxx
   
   dbgl_area.h                dbgl_area.txx

   dbgl_rigid_align.cxx       dbgl_rigid_align.h
   
   #######################################################################
   # internals
   
   # smallest enclosing balls of points
   # downloaded from http://www.inf.ethz.ch/personal/gaertner/miniball.html
   internals/miniball.h       internals/miniball.txx
   internals/miniball_config.h
   internals/wrapped_array.h
   #########################################################################
      
)

AUX_SOURCE_DIRECTORY(Templates dbgl_sources)

ADD_LIBRARY(dbgl ${dbgl_sources})
  
TARGET_LINK_LIBRARIES( dbgl vgl vcl vnl vnl_algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( algo )

#IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )

