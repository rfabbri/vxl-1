# Shock library
# algo/shock/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(shock_sources

BaseGUIGeometry.h
belements-bucketing.cpp
belements-bucketing.h
belements.h
belements.cpp
belements-dynval.cpp
belements-dynval.h
belements-extrinsic.cpp
biarc_shock.cpp
biarc_shock.h
boundary-bucketing.cpp
boundary-bucketing.h
boundary.cpp
boundary-dynval.cpp
boundary-dynval.h
boundary.h
bucket.cpp
bucket.h
bucketing-bndadding.cpp
canny_edge.cpp
canny_edge.h
CEDT.h
CEDT.cpp
delaunay.cpp
delaunay.h
dynamics.cpp
dyn-val.cpp
dyn-val.h
dyn-val-new.cpp
fileio.cpp
graphfunctions.cpp
intersections.cpp
intersections.h
ishock-bnd-related.cpp
ishock-bucketing.cpp
ishock-init-zerolength.cpp
ishock-common.cpp
ishock-common.h
ishock.cpp
ishock_sptr.h
ishock-dynval.cpp
ishock.h
ishock-init.cpp
quad.cpp
quad.h
scdm.cpp
scdm.h
sielements.cpp
sielements-extrinsic.cpp
sielements.h
sielements-info.cpp
sielements-salience.cpp
sym-transforms.cpp
sym-transforms.h
validation.cpp

ishock_boundary_linking.cxx

#extrinsic shock classes
xshock.cpp
xshock.h

#semi-intrinsic shock classes

IShockXShock.cpp
IShockXShock.h

curve_utils.h
curve_utils.cxx

)

AUX_SOURCE_DIRECTORY(Templates shock_sources)

ADD_LIBRARY(shock  ${shock_sources})

TARGET_LINK_LIBRARIES( shock extrautils vgl vcl vnl vbl )

