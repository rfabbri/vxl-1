#ifndef boxm_opt_opencl_utils_h_
#define boxm_opt_opencl_utils_h_

#include "bcl_cl.h"
#include <string>
#include <cstddef>

std::size_t RoundUp(int global_size,int group_size);
std::string error_to_string(cl_int  status );

#endif
