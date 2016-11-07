#ifndef psm_mfa_compute_factor_properties_h_
#define psm_mfa_compute_factor_properties_h_

#include <vcl_vector.h>
#include <vcl_set.h>

#include <vgl/vgl_point_2d.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_cell_id.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_simple_rgb_processor.h>

#include <psm/algo/psm_raytrace_function.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>


template <psm_apm_type APM>
class psm_mfa_compute_factor_properties_functor
{
public:
  psm_mfa_compute_factor_properties_functor(psm_cell_id cell_id, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img_window,
    vil_image_view<float> &cell_vis, vil_image_view<float> &vis_inf, vil_image_view<float> &cell_pre, vil_image_view<float> &cell_post, vil_image_view<float> &seg_len,
    vcl_set<psm_cell_id> &markov_blanket)
    : cell_id_(cell_id), img_window_(img_window), markov_blanket_(markov_blanket),
    alpha_integral_(img_window.ni(), img_window.nj()), past_cell_(img_window.ni(), img_window.nj()), vis_img_(vis_inf), cell_vis_img_(cell_vis), pre_img_(cell_pre), post_img_(cell_post), seg_len_img_(seg_len)
  {
    markov_blanket_.clear();

    seg_len_img_.fill(0.0f);
    cell_vis_img_.fill(0.0f);
    vis_img_.fill(1.0f);
    pre_img_.fill(0.0f);
    post_img_.fill(0.0f);
    alpha_integral_.fill(0.0f);

    past_cell_.fill(false);
  }


  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<PSM_AUX_NULL>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    markov_blanket_.insert(cell_id);

    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI  = psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, img_window_(i,j));
    // compute energy function value for case when observation taken from this cell
    const float E = (float)vcl_log(PI + 0.001);
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;

    if (past_cell_(i,j)) {
      // update post
      post_img_(i,j) += E * Omega;
    }
    else if (cell_id == cell_id_) {
      past_cell_(i,j) = true;
      // store seg len for this ray, used for weighted averaging later on
      seg_len_img_(i,j) = seg_len;
      // take snapshot of visibility probability
      cell_vis_img_(i,j) = vis_img_(i,j);
    }
    else {
      // update pre 
      pre_img_(i,j) +=  E * Omega;
    }
    // update visibility probabilty
    vis_img_(i,j) = vis_prob_end;

    return true;
  }

private:

  const psm_cell_id cell_id_;

  // the observation (cropped)
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img_window_;

  // true when cell has already been passed on ray
  vil_image_view<bool> past_cell_;

  // for accumulating various properties along the viewing rays
  vil_image_view<float> &seg_len_img_;
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> &cell_vis_img_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> &pre_img_;
  vil_image_view<float> &post_img_;

  // for keeping track of cells in markov blanket 
  vcl_set<psm_cell_id> &markov_blanket_;

};

//: Functor class to compute log(PI) for each pixel in a view
template<psm_apm_type APM>
class mfa_PI_functor
{
public:
  mfa_PI_functor(typename psm_apm_traits<APM>::apm_datatype const& appearance) : appearance_(appearance) {}

  void operator()(typename psm_apm_traits<APM>::obs_datatype obs, float& PI)
  {
    PI = (float)vcl_log(psm_apm_traits<APM>::apm_processor::prob_density(appearance_, obs) + 0.001);
  }
private:
  typename psm_apm_traits<APM>::apm_datatype const& appearance_;
};


//: Functor class to normalize expected image. assumes weights are >= 0
template<class T>
class weighted_average_functor
{
public:
  weighted_average_functor() : value_sum_(0), weight_sum_(0.0f) {}

  void operator()(T value, float& weight) 
  {
    value_sum_ += value*weight;
    weight_sum_ += weight;
  }


  T get_average()
  {
    if (weight_sum_ > 0) {
      return value_sum_ / weight_sum_;
    } 
    return T(0);
  }
private:
  
  T value_sum_;
  float weight_sum_;

};



template <psm_apm_type APM>
bool psm_mfa_compute_factor_properties(psm_scene<APM> &scene, vpgl_perspective_camera<double> const& procam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, psm_cell_id const& cell_id,  vcl_vector<typename psm_apm_traits<APM>::obs_datatype> &obs_vector, vcl_vector<float> &vis_vector, vcl_vector<float> &mfa_pre_vector, vcl_vector<float> &mfa_post_vector, vcl_vector<float> &seg_len_vector, vcl_set<psm_cell_id> &markov_blanket, bool black_background = false)
{

  // compute the bounding box of the cell
  vbl_bounding_box<double,3> cube = scene.cell_bounding_box(cell_id);
  vbl_bounding_box<double,2> projection;
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};
  // project cell into image
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        double u,v;
        procam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],u,v);
        //const unsigned int u_int = (unsigned int)u;//vcl_min((unsigned int)(vcl_max(u,0.0)),img.ni()-1);
        //const unsigned int v_int = (unsigned int)v;//vcl_min((unsigned int)(vcl_max(v,0.0)),img.nj()-1);
        projection.update(u,v);
      }
    }
  }
  if ( (projection.xmax() <= 0.0) || (projection.xmin() >= img.ni())
    || (projection.ymax() <= 0.0) || (projection.ymin() >= img.nj()) ) {
      return false;
  }

  const unsigned int xmin = (unsigned int)vcl_max(0.0,projection.xmin());
  const unsigned int ymin = (unsigned int)vcl_max(0.0, projection.ymin());
  const unsigned int xmax = (unsigned int)vcl_min(img.ni()-1.0, projection.xmax());
  const unsigned int ymax = (unsigned int)vcl_min(img.nj()-1.0, projection.ymax());

  const unsigned int xdim = xmax - xmin + 1;
  const unsigned int ydim = ymax - ymin + 1;

  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> cell_view = vil_crop(img,xmin,xdim,ymin,ydim);

  // create a new camera for the cropped region
  vpgl_calibration_matrix<double> const& K = procam.get_calibration();
  vgl_vector_2d<double> crop_corner(projection.xmin(), projection.ymin());
  vpgl_calibration_matrix<double> cropped_K(K);
  cropped_K.set_principal_point(K.principal_point() - crop_corner);

  vpgl_perspective_camera<double> view_cam(cropped_K, procam.get_camera_center(), procam.get_rotation());

  vil_image_view<float> cell_vis(cell_view.ni(), cell_view.nj());
  vil_image_view<float> vis_inf(cell_view.ni(), cell_view.nj());
  vil_image_view<float> cell_mfa_pre(cell_view.ni(), cell_view.nj());
  vil_image_view<float> cell_mfa_post(cell_view.ni(), cell_view.nj());
  vil_image_view<float> cell_seg_len(cell_view.ni(), cell_view.nj());

  
  psm_raytrace_function<psm_mfa_compute_factor_properties_functor<APM>, APM> raytrace_fn(scene, &view_cam, cell_view.ni(), cell_view.nj(), false);
  psm_mfa_compute_factor_properties_functor<APM> functor(cell_id, cell_view, cell_vis, vis_inf, cell_mfa_pre, cell_mfa_post, cell_seg_len, markov_blanket);
  raytrace_fn.run(functor);

  vil_image_view<float> PI_inf(cell_view.ni(), cell_view.nj());
  if (black_background) {
    vcl_cout << ".";
    psm_apm_traits<APM>::obs_datatype black(0.0f);
    float background_std_dev = 8.0f/255;
    typename psm_apm_traits<APM>::apm_datatype background_apm;
    psm_apm_traits<APM>::apm_processor::init_appearance(black, background_std_dev*background_std_dev, background_apm);

    mfa_PI_functor<APM> background_PI_func(background_apm);
    vil_transform2<typename psm_apm_traits<APM>::obs_datatype, float, mfa_PI_functor<APM> >(cell_view,PI_inf,background_PI_func);

    // add inf term to post
    vil_image_view<float> inf_term(cell_view.ni(), cell_view.nj());
    vil_math_image_product(vis_inf,PI_inf,inf_term);
    vil_math_image_sum(inf_term, cell_mfa_post, cell_mfa_post);
  }
  else {
    // no need to add anything to cell_mfa_post since the PI_inf contribution is always 0
    //PI_inf.fill(0.0f); // uniform distribution [0, 1] : log(1) = 0
  }

  // add one element to each vector for each pixel with non-zero segment length
  for (unsigned int i=0; i<cell_seg_len.ni(); ++i) {
    for (unsigned int j=0; j<cell_seg_len.nj(); ++j) {
      if (cell_seg_len(i,j) > 0.0f) {
        obs_vector.push_back(cell_view(i,j));
        vis_vector.push_back(cell_vis(i,j));
        mfa_pre_vector.push_back(cell_mfa_pre(i,j));
        mfa_post_vector.push_back(cell_mfa_post(i,j));
        seg_len_vector.push_back(cell_seg_len(i,j));
      }
    }
  }


  return true;
}

#endif

