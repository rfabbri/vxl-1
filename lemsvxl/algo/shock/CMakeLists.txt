# Shock library
# algo/shock/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(shock_sources

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

aux_source_directory(Templates shock_sources)

add_library(shock  ${shock_sources})

target_link_libraries( shock extrautils vgl vcl vnl vbl )

