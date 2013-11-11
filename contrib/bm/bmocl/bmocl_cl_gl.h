// This is bmocl_cl_gl.h
#ifndef bmocl_cl_gl_h
#define bmocl_cl_gl_h

//:
// This is a copy of bocl_cl_gl.h so that I can use this in my library
// without using bocl

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <CL/cl_gl.h>
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(UNIX) || defined(linux) 
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLDevice.h>
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#endif


#endif
