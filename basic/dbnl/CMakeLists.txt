# lemsvxlsrc/basic/dbnl/CMakeLists.txt
# The Development Brown Numerical Library

SET( dbnl_sources
                                       dbnl_min_max_3.h
    dbnl_cbrt.cxx                      dbnl_cbrt.h
    dbnl_fresnel.cxx                   dbnl_fresnel.h
    dbnl_sinc.cxx                      dbnl_sinc.h
    dbnl_newton_root.cxx               dbnl_newton_root.h
    dbnl_gaussian.h
    dbnl_math.h
    dbnl_angle.h
    dbnl_solve_quadratic_equation.h        dbnl_solve_quadratic_equation.cxx
    dbnl_solve_trigonometric_equation.h    dbnl_solve_trigonometric_equation.cxx
)

ADD_LIBRARY(dbnl ${dbnl_sources})

TARGET_LINK_LIBRARIES( dbnl vnl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( algo )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

