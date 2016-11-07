#ifndef bvam_io_world_params_h_
#define bvam_io_world_params_h_


#include <vsl/vsl_binary_io.h>
#include "../bvam_world_params.h"



//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvam_world_params const& params);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvam_world_params &params);








#endif

