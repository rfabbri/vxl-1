// This is bmocl_cl.h
#ifndef bmocl_cl_h
#define bmocl_cl_h
//:
// This is just a copy of bocl_cl.h so that I can use this in my personal directory without
// including bocl

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
    defined(UNIX) || defined(linux)
  #include <CL/cl.h>
  #include <CL/cl_gl.h>
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
  #include <OpenCL/cl.h>
  #include <OpenCL/cl_gl.h>
  #include <OpenCL/cl_gl_ext.h>
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #include <opencl.h>
#endif


#endif //bocl_cl_h
