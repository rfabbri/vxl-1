if(APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-gnu-static-float-init -stdlib=libstdc++" CACHE STRING "C++ build flags for LEMSVPE under OSX")
endif()
set(BUILD_SHARED_LIBS ON CACHE BOOL "Reduce link time by using shared libs in LEMSVPE")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Debug by default in LEMSVPE")

message("Try to compile core first, then the libs you need.")
