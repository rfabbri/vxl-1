// This is bmocl_utils.h
#ifndef bmocl_utils_h
#define bmocl_utils_h

//:
// This is a copy of bocl_utilis.h so that I can use this in my library without including bocl
#include "bmocl/bmocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>
//: function to pad the global threads if it is not exact multiple of the workgroup size
vcl_size_t RoundUp(int global_size,int group_size);
//: function to display decoded error message.
vcl_string error_to_string(cl_int  status );


#endif
