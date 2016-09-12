# contrib/mauro/cmd/CMakeLists.txt
# Command line programs for mauro


#set(hello_sources
#   hello.cxx
#)

set(mytest_sources
#   my_test_1.cxx
#   my_test_2.cxx
   my_test_3.cxx
)


####################################################
#add_executable(hello
#  hello.h   you would list this here if it existed
#  ${hello_sources}
#
#)
#
# add vxl components you wanna use:
#target_link_libraries( hello vcl )

####################################################
add_executable(my_test
#  mytest.h   you would list this here if it existed
  ${mytest_sources}

)

# add vxl components you wanna use:
#target_link_libraries( my_test vcl vnl )
#target_link_libraries( my_test vcl vil) 
target_link_libraries(my_test vcl vgl)
