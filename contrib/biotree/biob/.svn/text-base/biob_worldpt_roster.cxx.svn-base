#include "biob_worldpt_roster.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>

biob_worldpt_roster::~biob_worldpt_roster(){}



void biob_worldpt_roster::x_write_this(vcl_ostream& os){
  vsl_basic_xml_element elm("biob_worldpt_roster");
  elm.add_attribute("num_points", (int) num_points());
  elm.x_write_open(os);
  for (unsigned long int i=0; i < num_points(); i++) {
    x_write(os, point(biob_worldpt_index(i)), "biob_worldpt_roster_point");
  }
  elm.x_write_close(os);
}
