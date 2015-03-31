# This is brcv/mvg/dbecl/CMakeLists.txt
# Brown Epipolar Curve Library

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )

SET(dbecl_sources
    dbecl_epipole.cxx           dbecl_epipole.h            dbecl_epipole_sptr.h
    dbecl_episeg.cxx            dbecl_episeg.h             dbecl_episeg_sptr.h
    dbecl_episeg_point.cxx      dbecl_episeg_point.h       dbecl_episeg_point_sptr.h
    dbecl_episeg_from_curve_converter.cxx  dbecl_episeg_from_curve_converter.h
    dbecl_epiband.cxx dbecl_epiband.h
    dbecl_epiband_iterator.cxx dbecl_epiband_iterator.h
    dbecl_epiband_builder.cxx dbecl_epiband_builder.h
)

AUX_SOURCE_DIRECTORY(Templates dbecl_sources)

ADD_LIBRARY(dbecl ${dbecl_sources})

TARGET_LINK_LIBRARIES(dbecl vpgl dbgl_algo vbl vul vnl vil vsol vgl)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
