#ifndef psm_io_scene_h_
#define psm_io_scene_h_
//:
// \file

#include <vsl/vsl_binary_io.h>
#include <iostream>
#include <psm/psm_scene_base.h>


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, psm_scene_base const &scene);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, psm_scene_base &scene);

void vsl_print_summary(std::ostream &os, const psm_scene_base &scene);

void vsl_b_read(vsl_b_istream& is, psm_scene_base* p);

void vsl_b_write(vsl_b_ostream& os, const psm_scene_base* &p);
 
void vsl_print_summary(std::ostream& os, const psm_scene_base* &p);

#endif

