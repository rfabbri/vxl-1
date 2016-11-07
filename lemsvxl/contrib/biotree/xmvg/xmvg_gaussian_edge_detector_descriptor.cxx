#include "xmvg_gaussian_edge_detector_descriptor.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>

xmvg_gaussian_edge_detector_descriptor::xmvg_gaussian_edge_detector_descriptor() :
xmvg_filter_descriptor()
{
}

xmvg_gaussian_edge_detector_descriptor::xmvg_gaussian_edge_detector_descriptor (double sigma, vgl_point_3d<double> centre) :
sigma_(sigma), centre_(centre)
{
  assert(sigma_ > 0);

  name_ = vcl_string("gaussian_edge_detector");

  calculate_bounding_box();
}

xmvg_gaussian_edge_detector_descriptor::~xmvg_gaussian_edge_detector_descriptor()
{
}

void xmvg_gaussian_edge_detector_descriptor::calculate_bounding_box(void)
{
  double centroid[3] = {0.0, 0.0, 0.0};
  double dummy_point[3] = {0.0, 0.0, 0.0};
  box_.add(dummy_point);
  box_.set_centroid(centroid);
  box_.set_width(8*sigma_);
  box_.set_height(8*sigma_);
  box_.set_depth(8*sigma_);
}

void x_write(vcl_ostream& os, xmvg_gaussian_edge_detector_descriptor d, vcl_string name)
{
  vsl_basic_xml_element element(name);

  element.add_attribute("sigma", d.sigma());
  element.x_write_open(os);
  x_write(os, d.centre(), "centre");
  element.x_write_close(os);
}
