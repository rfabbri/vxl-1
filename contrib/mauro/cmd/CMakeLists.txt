# contrib/mauro/cmd/CMakeLists.txt
# Command line programs for mauro


SET(hello_sources
   hello.cxx
)


ADD_EXECUTABLE(hello
#  hello.h   you would list this here if it existed
  ${hello_sources}
)

# add vxl components you wanna use:
TARGET_LINK_LIBRARIES( hello vcl )

