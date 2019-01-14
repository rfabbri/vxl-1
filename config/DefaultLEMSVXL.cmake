if(APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-gnu-static-float-init" CACHE STRING "C++ build flags for LEMSVPE under OSX")
endif()
set(BUILD_SHARED_LIBS ON CACHE BOOL "Reduce link time by using shared libs in LEMSVPE")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Debug by default in LEMSVPE")
set(BUILD_CONTRIB ON CACHE BOOL "Build LEMS contrib area (eg, for edge_det and rfabbri/mw)")
set(VXL_DIR "../vxl-bin" CACHE STRING "Location of VXL build in LEMSVPE")
set(VXD_DIR "../vxd-bin" CACHE STRING "Location of VXD build in LEMSVPE")

# can be useful to set USE_BOOST 

message("Try to compile core first, then the libs you need.")
