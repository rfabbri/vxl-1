if(APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-gnu-static-float-init" CACHE STRING "C++ build flags for LEMSVPE under OSX")
endif()
set(BUILD_SHARED_LIBS ON CACHE BOOL "Reduce link time by using shared libs in LEMSVPE")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Debug by default in LEMSVPE")
set(VXL_DIR "../vxl-bin" CACHE STRING "Location of VXL build in LEMSVPE")
set(BUILD_UNMAINTAINED_LIBRARIES ON CACHE BOOL "Build libraries that are no longer actively maintained in LEMSVPE")

message("Tip: Try to compile core first, then the libs you need.")
