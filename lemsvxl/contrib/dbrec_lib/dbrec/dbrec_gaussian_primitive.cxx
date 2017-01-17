//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//

#include "dbrec_gaussian_primitive.h"
#include "dbrec_type_id_factory.h"
#include <brip/brip_vil_float_ops.h>
#include <vnl/vnl_math.h>
#include <brip/brip_vil_float_ops.h>
#include <dbrec/dbrec_part_context.h>
#include <vil/vil_new.h>
#include <dbrec/dbrec_visitor.h>
#include <bxml/bsvg/bsvg_element.h>
#include <vcl_limits.h>

dbrec_gaussian::dbrec_gaussian(unsigned type, float lambda0, float lambda1, float theta, bool bright) : 
  dbrec_part(type), lambda0_(lambda0), lambda1_(lambda1), theta_(theta), bright_(bright), fast_(false), cutoff_percentage_(0.01f) 
  { 
    //: for gaussian primitives, the directional angles are always in the range [-90,90], 
    //  if user supplies an angle out of this range, then it needs to be reverted to this range
    theta_ = brip_vil_float_ops::extrema_revert_angle(theta_); 
    initialize_mask(); 
  }

vcl_string dbrec_gaussian::string_identifier(float angle) const 
{ 
  float angle_r = brip_vil_float_ops::extrema_revert_angle(angle);
  vcl_stringstream ss; ss << "gaussian_" << lambda0_ << "_" << lambda1_ << "_" << angle_r;
  if (bright_) ss << "_bright"; 
  else ss << "_dark"; 
  return ss.str(); 
}

//: construct the bg mu and sigma images using mean and std_dev of the bg model. 
//  we model the bg operator response as a gaussian at every pixel, so mu image contains the mean of the gaussian at every pixel, and sigma contains its std dev
bool dbrec_gaussian::construct_bg_response_model_gauss(const vil_image_view<float>& mean_img, const vil_image_view<float>& std_dev_img, vil_image_view<float> &mu_img, vil_image_view<float> &sigma_img)
{
  unsigned ni = mean_img.ni();
  unsigned nj = mean_img.nj();

  //: find the response img for this operator
  vil_image_view<float> mean_res;
  if (fast_) 
    mean_res = brip_vil_float_ops::fast_extrema(mean_img, lambda0_, lambda1_, theta_, bright_, true, true);
  else
    mean_res = brip_vil_float_ops::extrema(mean_img, lambda0_, lambda1_, theta_, bright_, true, true);

  //: find the sd dev of the operator at every pixel
  vbl_array_2d<float> kernel; vbl_array_2d<bool> mask;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask);
  vil_image_view<float> std_dev_res = brip_vil_float_ops::std_dev_operator_method2(std_dev_img, kernel);

  if (mean_res.ni() != ni || mean_res.nj() != nj)
    return false;

  if (mu_img.ni()  != ni || mu_img.nj() != nj || sigma_img.ni() != ni || sigma_img.nj() != nj)
    return false;

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float m = mean_res(i,j,2);
      if (bright_)
        m = -m;

      float s = std_dev_res(i,j);
      //float s = std_dev_img(i,j);

      mu_img(i,j)= m;
      sigma_img(i,j)= s;
    }

  return true;
}


void dbrec_gaussian::initialize_mask()
{
  vbl_array_2d<float> fa;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, fa, mask_);
  unsigned nrows = fa.rows(), ncols = fa.cols();
  rj_ = (nrows-1)/2;
  ri_ = (ncols-1)/2;
}



vcl_ostream& dbrec_gaussian::print(vcl_ostream& out) const
{
  dbrec_part::print(out);
  out << "\t\tgaussian part " << lambda0_ << " " << lambda1_ << " " << theta_ << " ";
  if (bright_)
    out << "bright mask:\n";
  else 
    out << "dark mask:\n";
  //out << mask_ << "\n";
  //out << "\t\t-----------\n";
  return out;
}

//: find P(alpha in foreground): the probability that this operator alpha in foreground
//  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
float dbrec_gaussian::fg_prob_operator(const vil_image_view<float>& fg_prob_img, unsigned i, unsigned j)
{
  int ni = fg_prob_img.ni();
  int nj = fg_prob_img.nj();
  float max_prob = 0.0f;
  for (int jj=-rj_; jj<=rj_; ++jj) {
    for (int ii=-ri_; ii<=ri_; ++ii) {
      if (!mask_[jj+rj_][ii+ri_])
        continue;
      else {
        int iii = i+ii;
        int jjj = j+jj;
        if (iii >= 0 && iii < ni && jjj >= 0 && jjj < nj && fg_prob_img(iii, jjj)>max_prob)
          max_prob=fg_prob_img(iii, jjj);
      }
    }
  }
  return max_prob;
}

bool dbrec_gaussian::mask_operator(const vil_image_view<bool>& mask_img, unsigned i, unsigned j)
{
  int ni = mask_img.ni();
  int nj = mask_img.nj();
  bool in_max = false;
  for (int jj=-rj_; jj<=rj_; ++jj) {
    for (int ii=-ri_; ii<=ri_; ++ii) {
      if (!mask_[jj+rj_][ii+ri_])
        continue;
      else {
        int iii = i+ii;
        int jjj = j+jj;
        if (iii >= 0 && iii < ni && jjj >= 0 && jjj < nj && mask_img(iii, jjj))
          in_max=true;
      }
    }
  }
  return in_max;
}

//: dilate the given mask and write to the new output
void dbrec_gaussian::mask_dilate(const vil_image_view<bool>& mask_img, vil_image_view<bool>& mask_out)
{
  for (unsigned i = 0; i < mask_out.ni(); i++) {
    for (unsigned j = 0; j < mask_out.nj(); j++) {
      bool val = mask_operator(mask_img, i, j);
      mask_out(i,j) = val;
    }
  }
}

//: find P(alpha in foreground): the probability that this operator alpha in foreground
//  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
//  the mask is re-calculated based on the passed angle (angle is in degrees)
float dbrec_gaussian::fg_prob_operator_rotational(const vil_image_view<float>& fg_prob_img, unsigned i, unsigned j, float angle)
{
  //: locate the mask
  vcl_map<float, vbl_array_2d<bool> >::iterator it = masks_.find(angle);
  
  vbl_array_2d<bool>* mask;
  vbl_array_2d<bool> mask_g;
  if (it == masks_.end()) {
    vbl_array_2d<float> fa;
    brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, angle, fa, mask_g);
    mask = &mask_g;
    masks_[angle] = mask_g;
  } else 
    mask = &(it->second);

  int rj = (mask->rows()-1)/2; 
  int ri = (mask->cols()-1)/2;

  int ni = fg_prob_img.ni();
  int nj = fg_prob_img.nj();
  float max_prob = 0.0f;
  for (int jj=-rj; jj<=rj; ++jj) {
    for (int ii=-ri; ii<=ri; ++ii) {
      if (!(*mask)[jj+rj][ii+ri])
        continue;
      else {
        int iii = i+ii;
        int jjj = j+jj;
        if (iii >= 0 && iii < ni && jjj >= 0 && jjj < nj && fg_prob_img(iii, jjj)>max_prob)
          max_prob=fg_prob_img(iii, jjj);
      }
    }
  }
  return max_prob;
}

//: retrieve a cached mask, if the mask for this angle is not in the cache then compute and cache it
void dbrec_gaussian::get_mask(vbl_array_2d<bool>& mask, float theta)
{
  //: locate the mask
  vcl_map<float, vbl_array_2d<bool> >::iterator it = masks_.find(theta);
  if (it == masks_.end()) {
    vbl_array_2d<float> fa;
    brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta, fa, mask);
    masks_[theta] = mask;
  } else 
    mask = it->second;  
}

//: dilate the given fg_map wrt this operators mask and write to the new output
void dbrec_gaussian::fg_map_dilate(const vil_image_view<float>& fg_prob_img, vil_image_view<float>& fg_prob_out)
{
  for (unsigned i = 0; i < fg_prob_out.ni(); i++) {
    for (unsigned j = 0; j < fg_prob_out.nj(); j++) {
      float val = fg_prob_operator(fg_prob_img, i, j);
      fg_prob_out(i,j) = val;
    }
  }
}

//: allow for various visitors (encapsulated algos working with the meta-structure) visit the gaussian primitive
void dbrec_gaussian::accept(dbrec_visitor* v) 
{ 
  v->visit_gaussian_primitive(this); 
}
void dbrec_gaussian::visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const
{
  float rx = 0.0f, ry = 0.0f;
  if (lambda0_ < lambda1_) {
    rx = vis_rad; //lambda0_;
    ry = (vis_rad/lambda0_) * lambda1_;
  } else {
    ry = vis_rad;
    rx = (vis_rad/lambda1_) * lambda0_;
  }
  bsvg_ellipse* ell = new bsvg_ellipse(rx, ry); 
  ell->set_location(x, y);
  ell->set_rotation(theta_);
  ell->set_fill_color(color);
  ell->set_stroke_color("yellow");
  doc.add_element(ell);

  vcl_stringstream ss; ss << type_; 
  bsvg_text* t = new bsvg_text(ss.str());
  t->set_location(x, y+3*vis_rad);
  t->set_fill_color(color);
  doc.add_element(t);
}

void dbrec_gaussian::get_direction_vector(vnl_vector_fixed<float,2>& v) const
{
  //: the constructor ensures that theta_ is in [-90,90]. 
  double theta_rad = theta_*vnl_math::pi/180.0;
  float c = (float)vcl_cos(theta_rad);
  float s = (float)vcl_sin(theta_rad);
  // prepare the clockwise rotation matrix
  vnl_matrix_fixed<float, 2, 2> rot_matrix;
  rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
  //: if lambda0>=lambda1, then the direction vector aligns with x axis when theta = 0, otherwise y axis
  if (lambda0_ >= lambda1_) {
    v(0) = 1.0f; v(1) = 0.0f; 
  } else {
    v(0) = 0.0f; v(1) = 1.0f;
  }
  //: now rotate by theta
  v = rot_matrix*v;
  //: cast to zero if sufficiently small
  if ((float)vcl_abs(v(0)) < vcl_numeric_limits<float>::epsilon()) v(0) = 0.0f;
  if ((float)vcl_abs(v(1)) < vcl_numeric_limits<float>::epsilon()) v(1) = 0.0f;
}
//: a method to compute a direction vector for rotationally invariant parts given a particular absolute angle for its orientation
void dbrec_gaussian::get_direction_vector(float abs_theta, vnl_vector_fixed<float, 2>& v) const
{
  double theta_rad = abs_theta*vnl_math::pi/180.0;
  float c = (float)vcl_cos(theta_rad);
  float s = (float)vcl_sin(theta_rad);
  // prepare the clockwise rotation matrix
  vnl_matrix_fixed<float, 2, 2> rot_matrix;
  rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
  //: if lambda0>=lambda1, then the direction vector aligns with x axis when theta = 0, otherwise y axis
  if (lambda0_ >= lambda1_) {
    v(0) = 1.0f; v(1) = 0.0f; 
  } else {
    v(0) = 0.0f; v(1) = 1.0f;
  }
  //: now rotate by theta
  v = rot_matrix*v;
  //: cast to zero if sufficiently small
  if ((float)vcl_abs(v(0)) < vcl_numeric_limits<float>::epsilon()) v(0) = 0.0f;
  if ((float)vcl_abs(v(1)) < vcl_numeric_limits<float>::epsilon()) v(1) = 0.0f;
}

//: check the "true" part of the mask
bool check_equal(const vbl_array_2d<bool>& left, const vbl_array_2d<bool>& right) {

  vbl_array_2d<bool>::size_type nr = left.rows();
  vbl_array_2d<bool>::size_type nc = left.cols();
  vbl_array_2d<bool>::size_type i1, j1;

  //: first check if they have the same number of true pixels
  int cnt_left = 0;
  for (i1 = 0; i1 < left.rows(); i1++) {
    for (j1 = 0; j1 < left.cols(); j1++) {
      if (left.get_rows()[i1][j1])
        cnt_left++;
    }
  }
  int cnt_right = 0;
  for (i1 = 0; i1 < right.rows(); i1++) {
    for (j1 = 0; j1 < right.cols(); j1++) {
      if (right.get_rows()[i1][j1])
        cnt_right++;
    }
  }
  if (cnt_left != cnt_right)
    return false;

  // find the first "true" pixel on both masks and use that pixel to centralize the second part on top of the other one
  bool found_it = false;
  for (i1 = 0; i1 < left.rows(); i1++) {
    for (j1 = 0; j1 < left.cols(); j1++) {
      if (left.get_rows()[i1][j1]) {
        found_it = true; 
        break;
      }
    }
    if (found_it)
      break;
  }
  if (!found_it)
    return false;

  vbl_array_2d<bool>::size_type i2, j2;
  found_it = false;
  for (i2 = 0; i2 < right.rows(); i2++) {
    for (j2 = 0; j2 < right.cols(); j2++) {
      if (right.get_rows()[i2][j2]) {
        found_it = true; 
        break;
      }
    }
    if (found_it)
      break;
  }
  if (!found_it)
    return false;

  //: now we think i1,j1 and i2,j2 correspond
  vbl_array_2d<bool>::size_type i11, j11;
  for (i11 = 0; i11 < left.rows(); i11++) {
    for (j11 = 0; j11 < left.cols(); j11++) {
      if (left.get_rows()[i11][j11]) {
        //: we expect right to have a true value at the corresponding spot when centered on i1, j2, otherwise they're not equal
        int i22, j22;
        i22 = i11-i1+i2;
        j22 = j11-j1+j2;
        if (i22 < 0 || i22 >= (int)right.rows() || j22 < 0 || j22 >= (int)right.cols() || !right.get_rows()[i22][j22])
          return false;
      }
    }
  }
  
  return true;
}

//: define equivalence as the equivalence of the masks
bool dbrec_gaussian::equal(const dbrec_gaussian& other) const
{
  return (this->bright_ == other.bright_ && check_equal(this->mask_, other.mask_));
}

//: mass of the gaussian primitives is the area of their mask
float dbrec_gaussian::mass() const
{
  //: first check if they have the same number of true pixels
  int cnt = 0;
  for (unsigned i1 = 0; i1 < mask_.rows(); i1++) {
    for (unsigned j1 = 0; j1 < mask_.cols(); j1++) {
      if (mask_.get_rows()[i1][j1])
        cnt++;
    }
  }
  return (float)cnt;
}


//: add an instance with the given parameters to the factory
//  primitives with the same masks are considered to be equivalent, makes sure primitives are not repeated, e.g. {lambda0, lambda1, 0, bright} == {lambda1, lambda0, 90, bright}
void dbrec_gaussian_factory::add(float lambda0, float lambda1, float theta, bool bright, bool fast)
{
  dbrec_gaussian* p = new dbrec_gaussian(0, lambda0, lambda1, theta, bright);
  if (fast)
    p->set_extraction_method_to_fast();
  for (part_const_iterator it = parts_const_begin(); it != parts_const_end(); it++) {
    dbrec_gaussian* p1 = dynamic_cast<dbrec_gaussian*>(it->second.ptr());
    if (p1->equal(*p))
      return;  // don't add this part
  }
  p->reset_type(dbrec_type_id_factory::instance()->new_type());
  add_primitive(p);

}
void dbrec_gaussian_factory::add(dbrec_gaussian* g)
{
  for (part_const_iterator it = parts_const_begin(); it != parts_const_end(); it++) {
    dbrec_gaussian* p1 = dynamic_cast<dbrec_gaussian*>(it->second.ptr());
    if (p1->equal(*g))
      return;  // don't add this part
  }
  add_primitive(g);
}

//: create instances with the given number of directions, lambda range and with lambda increments
void dbrec_gaussian_factory::populate(int ndirs, float lambda_range, float lambda_inc, bool bright, bool fast)
{
  //: lambda_min = lambda_inc, lambda_max = lambda_range, lambda_inc = lambda_inc
  this->populate(ndirs, lambda_inc, lambda_range, lambda_inc, bright, fast);
}

void dbrec_gaussian_factory::populate(int ndirs, float lambda_min, float lambda_max, float lambda_inc, bool bright, bool fast)
{
  float theta = 0.0f;
  float theta_inc = 180.0f/ndirs;
  unsigned type_cnt = 0;

  for (float lambda0 = lambda_min; lambda0 <= lambda_max; lambda0 += lambda_inc) {
    for (float lambda1 = lambda_min; lambda1 <= lambda_max; lambda1 += lambda_inc) {
      theta = 0.0f;
      for (int i = 0; i < ndirs; i++) {
        //: initialize bright operator
        float adjusted_theta = theta > 90 ? -(180-theta) : theta;  // operators work in [-pi, pi] range
        add(lambda0, lambda1, adjusted_theta, bright, fast);
        theta += theta_inc;
      }
    }
  }
}

void dbrec_gaussian_factory::populate_rot_inv(float lambda_min, float lambda_max, float lambda_inc, bool bright, bool fast)
{
  float theta = 0.0f;
  unsigned type_cnt = 0;
  vcl_vector<float> lambdas;
  for (float lambda = lambda_min; lambda <= lambda_max; lambda += lambda_inc) 
    lambdas.push_back(lambda);
  for (unsigned i = 0; i < lambdas.size(); i++) {
    for (unsigned j = i; j < lambdas.size(); j++) {
      //adjust lambdas, lambda0 is assumed to be larger than lambda1 during parsing
      if (lambdas[i] < lambdas[j])
        add(lambdas[j], lambdas[i], theta, bright, fast);
      else
        add(lambdas[i], lambdas[j], theta, bright, fast);
    }
  }

}

