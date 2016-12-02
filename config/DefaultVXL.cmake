if(APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-gnu-static-float-init -stdlib=libstdc++" CACHE STRING "C++ build flags for LEMSVPE under OSX")
endif()
set(BUILD_NONDEPRECATED_ONLY OFF CACHE BOOL "Build deprecated needed for LEMSVPE")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Reduce link time by using shared libs in LEMSVPE")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Debug by default in LEMSVPE")
set(VNL_CONFIG_LEGACY_METHODS ON CACHE BOOL "Whether backward-compatibility methods are provided by vnl. On in LEMSVPE")


message("Try to compile core first. Then Turn on VGUI in CMake and try to build it.")
