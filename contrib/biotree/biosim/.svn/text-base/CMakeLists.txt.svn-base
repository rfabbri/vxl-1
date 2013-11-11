INCLUDE_DIRECTORIES(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/brcv/mvg ${lems_SOURCE_DIR}/basic/dbsta2)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc ${VXL_MUL_INCLUDE_DIR})

SET(biosim_src
biosim_ctsim.h              biosim_ctsim.cxx
biosim_cylinder_pyramid.h   biosim_cylinder_pyramid.cxx
biosim_deformed_torus.h     biosim_deformed_torus.cxx
)

AUX_SOURCE_DIRECTORY(Templates biosim_src)
ADD_LIBRARY(biosim ${biosim_src})
TARGET_LINK_LIBRARIES(biosim xmvg xscan biocts vpgl vpgl_algo vgl vil vil3d vbl vnl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
