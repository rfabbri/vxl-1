#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <psm/psm_scene_base.h>


#include "psm_io_scene.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, psm_scene_base const &scene)
{
  vcl_cerr << "warning: vsl_b_write not implemented for psm_scene_base" << vcl_endl;
}

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, psm_scene_base &scene)
{
    vcl_cerr << "warning: vsl_b_read not implemented for psm_scene_base" << vcl_endl;
}

void vsl_print_summary(vcl_ostream &os, const psm_scene_base &scene)
{
  os << "origin = " << scene.origin() << ", block_len = " << scene.block_len();
}

void vsl_b_read(vsl_b_istream& is, psm_scene_base* p)
{
  vcl_cerr << "warning: vsl_b_read not implemented for psm_scene_base*" << vcl_endl;
}

void vsl_b_write(vsl_b_ostream& os, const psm_scene_base* &p)
{
  vcl_cerr << "warning: vsl_b_write not implemented for psm_scene_base*" << vcl_endl;
}
 
void vsl_print_summary(vcl_ostream& os, const psm_scene_base* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}

