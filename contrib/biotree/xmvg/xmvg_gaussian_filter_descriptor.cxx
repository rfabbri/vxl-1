#include "xmvg_gaussian_filter_descriptor.h"
#include <xmvg/xmvg_filter_descriptor.h>
#include <vsl/vsl_basic_xml_element.h>

xmvg_gaussian_filter_descriptor::xmvg_gaussian_filter_descriptor() :
xmvg_filter_descriptor()
{
}

xmvg_gaussian_filter_descriptor::xmvg_gaussian_filter_descriptor (double sigma_r, double length,
                                                                  vgl_point_3d<double> centre, 
                                                                  vgl_vector_3d<double> orientation) :
sigma_r_(sigma_r), sigma_z_(length/2)
{
  assert(sigma_r_ > 0.);
  assert(sigma_z_ > 0.);
  // Orientation vector cannot be (0,0,0).
  assert(orientation.x() != 0 || orientation.y() != 0 || orientation.z() != 0);

  name_ = vcl_string("gaussian_filter");
  centre_ = centre;
  // Normalize the orientation vector.
  orientation_ = normalize(orientation);
  // Calculate the rotation parameters from (0,0,1) to the orientation of the cylinder.
  // Rotation angle is obtained from the dot product of (0,0,1) and orientation.
  // Rotation axis is obtained from the vector product of (0,0,1) and orientation.
  // You may think that the rotation axis being (0,0,0) may cause trouble, but it does not
  // here for the rest of the code, and I did not want to add an if statement because of
  // time and performance constraints. However, this should be one of the first places to
  // look in case of a crash, or something related to xmvg not working fine. (06/26/05)
  rotation_angle_ = vcl_acos(orientation_.z());
  rotation_axis_ = vnl_vector_fixed<double,3>(-orientation_.y(), orientation_.x(), 0);
  // The transformation matrix
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  tr.set_rotation_about_axis(rotation_axis_, rotation_angle_);

  calculate_bounding_box();
}

xmvg_gaussian_filter_descriptor::~xmvg_gaussian_filter_descriptor()
{
}

#if 0

void xmvg_gaussian_filter_descriptor::calculate_bounding_box(void)
{
  double centroid[3] = {0.0, 0.0, 0.0};
  double dummy_point[3] = {0.0, 0.0, 0.0};
  box_.add(dummy_point);
  box_.set_centroid(centroid);
  box_.set_width(8*sigma_r_);
  box_.set_height(8*sigma_r_);
  box_.set_depth(6*sigma_z_);
}

#endif

void xmvg_gaussian_filter_descriptor::calculate_bounding_box(void)
    {

  double centroid[3] = {0.0, 0.0, 0.0};
  double dummy_point[3] = {0.0, 0.0, 0.0};
  box_.add(dummy_point);
  box_.set_centroid(centroid);

    double cos_x,cos_y,sig_r,sig_z,x_margin,y_margin;

    cos_x = orientation().x();
    cos_y = orientation().y();
    sig_r = sigma_r();
    sig_z = sigma_z();

    x_margin = sig_z*cos_x + 8*sig_r;

    y_margin = sig_z*cos_y + 8*sig_r;

    box_.set_width(x_margin);
    box_.set_height(y_margin);
    box_.set_depth(6*sig_z);

    }

void x_write(vcl_ostream& os, xmvg_gaussian_filter_descriptor f)
{ 
  //: open the tag
  vsl_basic_xml_element name_element("xmvg_gaussian_filter_descriptor");
  name_element.x_write_open(os);

  //:write child elements
  vcl_string name = "filter_desc";
  x_write(os, static_cast<xmvg_filter_descriptor&>(f)); 

  vsl_basic_xml_element r("sigma_r");
  r.append_cdata(f.sigma_r());
  r.x_write(os);

  vsl_basic_xml_element z("sigma_z");
  z.append_cdata(f.sigma_z());
  z.x_write(os);

  //: close the tag
  name_element.x_write_close(os);
}
