#ifndef breg3d_util_h_
#define breg3d_util_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>

#include <vil/vil_image_view.h>

#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_sample_grid_bilin.h>

//: This class contains static utility methods related to the breg3d 3D registration library
class breg3d_util 
{
public:

  static bool read_cameras(const char* filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts);

  static bool write_cameras(const char* filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts);

  static vil_image_view<float> load_image(vcl_string img_fname);
  static void save_image(vil_image_view<float> img, vcl_string img_fname);

  template<class T>
  static bool resample_image(vil_image_view<T> const& in_view, vil_image_view<T> &out_view, vimt_transform_2d &xform, double off_x = 0.0, double off_y = 0.0);


};

template<class T>
bool breg3d_util::resample_image(vil_image_view<T> const& in_view, vil_image_view<T> &out_view, vimt_transform_2d &xform, double off_x, double off_y)
{
  vimt_image_2d_of<T> view_of_in(in_view,xform);

  vgl_point_2d<double>  offset(off_x,off_y);
  vgl_vector_2d<double> u_vec(0,1);
  vgl_vector_2d<double> v_vec(1,0);

  vnl_vector<T> out_vec(out_view.size());
  // seeing something weird here
  if (!out_vec.data_block()) {
    vcl_cerr << "failed to create vector of size " << out_view.size() << ". " << vcl_endl;
  }


  vimt_sample_grid_bilin(out_vec,view_of_in,offset,u_vec,v_vec, out_view.nj(),out_view.ni());
  // copy vec into image view
  T *vec_ptr = out_vec.data_block();
  if (out_view.is_contiguous()) {
    for (vil_image_view<T>::iterator vit = out_view.begin(); vit != out_view.end(); vit++) {
      *vit = *vec_ptr++;
    }
  }
  else {
    // out_view is not contiguous (eg a crop), so we cant use the iterators.
    if (out_view.istep() == 1) {
      // might be able to get a little speed up by using pointer incrememnt instead of adding istep()
      T* img_ptr = out_view.top_left_ptr();
      for (unsigned j=0; j < out_view.nj(); j++) {
        for (unsigned i=0; i < out_view.ni(); i++) {
          *img_ptr++ = *vec_ptr++; // assumes out_view.istep() == 1
        }
        img_ptr += out_view.jstep() - (out_view.istep()*out_view.ni());
      }
    }
    else {
      // general case, add istep / jstep after visiting each pixel / row
      T* img_ptr = out_view.top_left_ptr();
      for (unsigned j=0; j < out_view.nj(); j++) {
        for (unsigned i=0; i < out_view.ni(); i++) {
          *img_ptr++ = *vec_ptr++; 
          img_ptr += out_view.istep();
        }
        img_ptr += out_view.jstep() - (out_view.istep()*out_view.ni());
      }
    }
  }

  return true;

}

#endif

