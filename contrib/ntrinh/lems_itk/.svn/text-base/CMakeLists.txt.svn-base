# This project is designed to be built outside the Insight source tree.
PROJECT(lems_itk)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
  ADD_EXECUTABLE(HelloWorld HelloWorld.cxx )
  TARGET_LINK_LIBRARIES(HelloWorld ITKCommon)
  
  SUBDIRS( surfmatch )
ENDIF(ITK_FOUND)
