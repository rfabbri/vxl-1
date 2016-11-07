#include <dber/dber_edgel_similarity.h>

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_distance.h>

//#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>


//: compute current matching cost of matching these two edgels
//  small if spatially distant and orientatations are different
//  if orthogonal this is zero, (no polarizations)
double dber_edgel_similarity::compute_current_cost(vsol_line_2d_sptr e1, vsol_line_2d_sptr e2, double sigma_square, double & max)
{
  vsol_point_2d_sptr mid1 = e1->middle();
  vsol_point_2d_sptr mid2 = e2->middle();

  // purely geometric
  double angle1 = e1->tangent_angle();
  double angle2 = e2->tangent_angle();
  if (angle1 > 180.0f) 
    angle1 = vcl_fmod(angle1 + 180.0f, double(360.0f));
  if (angle2 > 180.0f) 
    angle1 = vcl_fmod(angle1 + 180.0f, double(360.0f));

  double angle_dif = vcl_abs(angle1 - angle2);
  if (angle_dif > 90.0f)
    angle_dif = 180.0f - angle_dif;
  //vcl_cout << "angle dif: " << angle_dif  << " degrees ";
  double cos_angle = vcl_cos(angle_dif*vnl_math::pi/180.0f);
  double dot_p = e1->length()*e2->length()*cos_angle;
  double spatial_dist = vgl_distance(mid1->get_p(), mid2->get_p());
  double d = vcl_exp(-(spatial_dist*spatial_dist)/sigma_square);
  max = d;  // not considering orientations
  return d*dot_p;
}

//: measure norm between lines1 and lines2 wrt current-matching norm
double dber_edgel_similarity::current_norm(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square) {
  int s1 = l1.size();
  int s2 = l2.size();
  if (!s1 || !s2)
    return -1;
  
  double self_sim1 = 0;
  for (int i = 0; i<s1; i++)
    for (int j = 0; j<s1; j++) {
      double max;
      self_sim1 += compute_current_cost(l1[i], l1[j], sigma_square, max);
    }

  double self_sim2 = 0;
  for (int i = 0; i<s2; i++)
    for (int j = 0; j<s2; j++) {
      double max;
      self_sim2 += compute_current_cost(l2[i], l2[j], sigma_square, max);
    }

  //: Ozge TODO: Implementation here is a little different than Joan Glaunes's Matlab Implementation
  //  check this!
  double cost = 0;
  for (int i = 0; i<s1; i++)
    for (int j = 0; j<s2; j++) {
      double max;
      cost += compute_current_cost(l1[i], l2[j], sigma_square, max);
    }

  // return the norm
  return self_sim1 + self_sim2 - cost;
}

//: measure support for lines1 by lines2 wrt current edgel similarity measure
double dber_edgel_similarity::measure_support(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square) {
  int s1 = l1.size();
  int s2 = l2.size();
  if (!s1 || !s2)
    return -1;
   
  double cost = 0;
  double max_cost = 0;
  for (int i = 0; i<s1; i++)
    for (int j = 0; j<s2; j++) {
      double max;
      cost += compute_current_cost(l1[i], l2[j], sigma_square, max);
      max_cost += max;
    }

  // normalize the cost with max possible value
  return cost/max_cost;
}

//: get image sets for each edgel sets and prepare gradient images, etc.
void dber_edgel_similarity::prepare_images(vcl_vector<vil_image_view<vxl_byte> >&set1, 
                                           vcl_vector<vil_image_view<vxl_byte> >&set2, float smoothing_sigma) {

  // for now, later will come from VJ's new class
  //nplanes_ = im1_->nplanes();
  //vil_image_view<vxl_byte> im1_view = im1_->get_view();
  // treat each plane as coming from a different frame for now
  //for (unsigned i = 0; i<nplanes_; i++) {
    //vil_image_view<vxl_byte> view1 = vil_plane(im1_view, i);
    //im1_views_.push_back(view1);

  nplanes_ = (set1.size() < set2.size()) ? set1.size() : set2.size();
  for (unsigned i = 0; i <nplanes_; i++) {  
    vil_image_view<vxl_byte> view1 = set1[i];
    im1_views_.push_back(view1);
    vil_image_view<vxl_byte> view2 = set2[i];
    im2_views_.push_back(view2);

    vil_image_view<float> fimage1; vil_convert_cast(view1, fimage1);
    vil_image_view<float> fimage2; vil_convert_cast(view2, fimage2);
    //smooth the image
    vil_image_view<float> gimage1 = brip_vil_float_ops::gaussian(fimage1, smoothing_sigma);
    vil_image_view<float> gimage2 = brip_vil_float_ops::gaussian(fimage2, smoothing_sigma);
    unsigned ni = fimage1.ni(), nj = fimage1.nj();
    vil_image_view<float> Ix1(ni, nj), Iy1(ni, nj);
    brip_vil_float_ops::gradient_3x3(gimage1, Ix1, Iy1);
    I1xs_.push_back(Ix1); I1ys_.push_back(Iy1);

    ni = fimage2.ni(); nj = fimage2.nj();
    vil_image_view<float> Ix2(ni, nj), Iy2(ni, nj);
    brip_vil_float_ops::gradient_3x3(gimage2, Ix2, Iy2);
    I2xs_.push_back(Ix2); I2ys_.push_back(Iy2);
  }
}

void dber_edgel_similarity::update_histograms_from_all_planes(int x1, int y1, int x2, int y2, 
                                                   bsta_histogram<float>& h1, 
                                                   bsta_histogram<float>& h2, 
                                                   bsta_joint_histogram<float>& jh) 
{
  float deg_rad = (float)(180.0/vnl_math::pi);
  for (unsigned i = 0; i<nplanes_; i++) {
    float Ix1 = I1xs_[i](x1, y1);
    float Iy1 = I1ys_[i](x1, y1);
    float Ix2 = I2xs_[i](x2, y2);
    float Iy2 = I2ys_[i](x2, y2);
    float ang1 = deg_rad*vcl_atan2(Iy1, Ix1) + 180.0f;
    float ang2 = deg_rad*vcl_atan2(Iy2, Ix2) + 180.0f;
    float mag1 = vcl_abs(Ix1)+vcl_abs(Iy1);
    float mag2 = vcl_abs(Ix2)+vcl_abs(Iy2);
    h1.upcount(ang1, mag1); h2.upcount(ang2, mag2);
    jh.upcount(ang1, mag1, ang2, mag2);
  }
}

//: compute cost of matching these two edgels
//  find the gradient direction mutual info between 
//  regions given by coordinate systems centered on these edgels
double dber_edgel_similarity::compute_cost(vsol_line_2d_sptr e1, vsol_line_2d_sptr e2, double width_radius, double radius)
{
  vsol_point_2d_sptr midp1 = e1->middle();
  vsol_point_2d_sptr midp2 = e2->middle();

  // don't bother if they're too far away
  double spatial_dist = vgl_distance(midp1->get_p(), midp2->get_p());
  if (spatial_dist > width_radius)
    return -1;

  // find the gradient direction mutual info
  float range = 360.0;
  unsigned bins = 8;
  float deg_rad = (float)(180.0/vnl_math::pi);
  bsta_histogram<float> h1(range, bins), h2(range, bins);
  bsta_joint_histogram<float> jh(range, bins);
  
  int x1 = (int)vcl_floor(midp1->x()+0.5);
  int y1 = (int)vcl_floor(midp1->y()+0.5);
  int x2 = (int)vcl_floor(midp2->x()+0.5);
  int y2 = (int)vcl_floor(midp2->y()+0.5);

  //update_histograms_from_all_planes(x1, y1, x2, y2, h1, h2, jh);
  
  // all pixels on each ray, corresponds to all pixels on the second ray
  // collect correspondences in the histograms
  //TODO: check the image borders
  int upper = (int)vcl_floor(radius+0.5);
  int lower = (int)vcl_floor(-radius+0.5);
  for(int i = 0; i<=upper; ++i)
    for (int j = 0; j<=upper; j++) 
      update_histograms_from_all_planes(x1, y1+i, x2, y2+j, h1, h2, jh);

  for(int i = lower; i<0; ++i)
    for (int j = lower; j<0; j++) 
      update_histograms_from_all_planes(x1, y1+i, x2, y2+j, h1, h2, jh);
  
  for(int i = 0; i<=upper; ++i)
    for (int j = 0; j<=upper; j++) 
      update_histograms_from_all_planes(x1+i, y1, x2+j, y2, h1, h2, jh);

  for(int i = lower; i<0; ++i)
    for (int j = lower; j<0; j++) 
      update_histograms_from_all_planes(x1+i, y1, x2+i, y2, h1, h2, jh);
  
  float H1 = h1.entropy(), H2 = h2.entropy();
  float H12 = jh.entropy();
  float mi = H1 + H2 - H12;
  
  vcl_cout << "mi: " << mi << vcl_endl;
  return mi;
}

