set(crvmtch3D_src
Curve.h Curve.C
DPMatch.h DPMatch.C
NewDPMatch.h NewDPMatch.C
Point.h
test.h test.C
utils.h utils.C
)

aux_source_directory(Templates crvmtch3D_src)

add_library(crvmtch3D ${crvmtch3D_src})

target_link_libraries(crvmtch3D vcl)

if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)
