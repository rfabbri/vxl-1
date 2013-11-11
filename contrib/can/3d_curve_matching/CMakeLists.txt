SET(crvmtch3D_src
Curve.h Curve.C
DPMatch.h DPMatch.C
NewDPMatch.h NewDPMatch.C
Point.h
test.h test.C
utils.h utils.C
)

AUX_SOURCE_DIRECTORY(Templates crvmtch3D_src)

ADD_LIBRARY(crvmtch3D ${crvmtch3D_src})

TARGET_LINK_LIBRARIES(crvmtch3D vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
