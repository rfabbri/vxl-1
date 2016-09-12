# This project is designed to be built outside the Insight source tree.
project(lems_itk)

# Find ITK.
find_package(ITK)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
  add_executable(HelloWorld HelloWorld.cxx )
  target_link_libraries(HelloWorld ITKCommon)
  
  subdirs( surfmatch )
endif(ITK_FOUND)
