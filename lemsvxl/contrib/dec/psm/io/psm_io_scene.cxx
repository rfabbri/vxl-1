#include <vsl/vsl_binary_io.h>
#include <iostream>
#include <psm/psm_scene_base.h>


#include "psm_io_scene.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, psm_scene_base const &scene)
{
  std::cerr << "warning: vsl_b_write not implemented for psm_scene_base" << std::endl;
}

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, psm_scene_base &scene)
{
    std::cerr << "warning: vsl_b_read not implemented for psm_scene_base" << std::endl;
}

void vsl_print_summary(std::ostream &os, const psm_scene_base &scene)
{
  os << "origin = " << scene.origin() << ", block_len = " << scene.block_len();
}

void vsl_b_read(vsl_b_istream& is, psm_scene_base* p)
{
  std::cerr << "warning: vsl_b_read not implemented for psm_scene_base*" << std::endl;
}

void vsl_b_write(vsl_b_ostream& os, const psm_scene_base* &p)
{
  std::cerr << "warning: vsl_b_write not implemented for psm_scene_base*" << std::endl;
}
 
void vsl_print_summary(std::ostream& os, const psm_scene_base* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}

