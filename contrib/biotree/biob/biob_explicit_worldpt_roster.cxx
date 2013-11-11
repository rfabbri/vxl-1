#include <biob/biob_explicit_worldpt_roster.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>

biob_worldpt_index biob_explicit_worldpt_roster::add_point(worldpt pt){
  points_.push_back(pt);
  return biob_worldpt_index(points_.size()-1);
}
   
worldpt biob_explicit_worldpt_roster::point(biob_worldpt_index pti) const{
  return points_[pti.index()];
}

unsigned long int biob_explicit_worldpt_roster::num_points() const{
  return points_.size();
}

void biob_explicit_worldpt_roster::x_write_this(vcl_ostream &os) {
  vsl_basic_xml_element elm("biob_explicit_worldpt_roster");
  elm.add_attribute("num_points", (int) num_points());
  elm.x_write_open(os);
  vcl_vector<worldpt> the_points = points();
  for (unsigned long int i=0; i < num_points(); i++) {
    x_write(os, the_points[i], "biob_explicit_worldpt_roster_points");
  }
  elm.x_write_close(os);

}
/*
void biob_explicit_worldpt_roster::read(vsl_b_istream & is){
  unsigned long int num_points;
  vsl_b_read(is, num_points);
  points_.resize(num_points);
  for (unsigned long int i = 0; i < num_points; ++i){
    vsl_b_read(is, points_[i]);
  }
}
*/
