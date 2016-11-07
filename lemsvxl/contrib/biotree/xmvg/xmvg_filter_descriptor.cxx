// This is /contrib/biotree/xmvg/xmvg_filter_descriptor.cxx
#include "xmvg_filter_descriptor.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>
//#include <proc/io/proc_io_xml_parser.h>

void x_write(vcl_ostream & os, xmvg_filter_descriptor f)
{
  // child will write the descriptors opening tag

  vsl_basic_xml_element element("xmvg_filter_descriptor");
  element.x_write_open(os);

  vsl_basic_xml_element name_element("name");
  name_element.append_cdata(f.name());
  name_element.x_write(os);
  //os << "<name>" << f.name() << "</name>" << "\n";

  x_write(os, f.box(), "box");
  x_write(os, f.centre(), "centre");
  x_write(os, f.orientation(), "orientation");

  //: <rotation_angle>0.3<rotation_angle/>/>
  vsl_basic_xml_element rot_angle_element("rotation_angle");
  rot_angle_element.append_cdata(f.rotation_angle());
  rot_angle_element.x_write(os);

  //: <rotation_axis x="1" y="1" z="1"/>
  /*os << "<rotation_axis x=\"" << f.rotation_axis().get(0) << 
    "\" y=\"" << f.rotation_axis().get(1) << 
    "\" z=\"" << f.rotation_axis().get(2) << "\"/>" << "\n";*/
  x_write(os, f.rotation_axis(), "rotation_axis");  

  //os << "</" << element_name << ">" << "\n";
  element.x_write_close(os);
}
