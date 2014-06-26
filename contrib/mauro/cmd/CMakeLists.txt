# contrib/mauro/cmd/CMakeLists.txt
# Command line programs for mauro


#SET(hello_sources
#   hello.cxx
#)

SET(mytest_sources
#   my_test_1.cxx
#   my_test_2.cxx
   my_test_3.cxx
)


####################################################
#ADD_EXECUTABLE(hello
#  hello.h   you would list this here if it existed
#  ${hello_sources}
#
#)
#
# add vxl components you wanna use:
#TARGET_LINK_LIBRARIES( hello vcl )

####################################################
ADD_EXECUTABLE(my_test
#  mytest.h   you would list this here if it existed
  ${mytest_sources}

)

# add vxl components you wanna use:
#TARGET_LINK_LIBRARIES( my_test vcl vnl )
#TARGET_LINK_LIBRARIES( my_test vcl vil) 
TARGET_LINK_LIBRARIES(my_test vcl vgl)
