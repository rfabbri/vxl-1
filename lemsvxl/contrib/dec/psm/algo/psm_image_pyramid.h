#ifndef psm_image_pyramid_h_
#define psm_image_pyramid_h_

#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_bilin_interp.h>
#include <vil/algo/vil_gauss_reduce.h>

template<class T>
class psm_image_pyramid
{

public:
  psm_image_pyramid(vil_image_view<T> &base_img) 
  {
    images_.clear();
    images_.push_back(base_img);
    build_pyramid();
    update_pyramid();
  }

  unsigned int nlevels() {return images_.size();}

  T operator()(double i, double j, unsigned int plane, unsigned int level)
  {
    if (level >= images_.size()) {
      vcl_cerr << "error: psm_image_pyramid: asked for level " << level <<", images_.size() == " << images_.size() << vcl_endl;
      return T(0);
    }
    double scaled_i = i / (1 << level); 
    double scaled_j = j / (1 << level);

    return vil_bilin_interp_safe(images_[level],scaled_i,scaled_j,plane);
  }

  vil_image_view<T>& get_base() { return images_[0]; }

  //: call this function after modifying the base image.
  void update_pyramid() {
    vil_image_view<T> work_img(images_[1].ni(), images_[1].nj(), images_[1].nplanes());
    for (unsigned int i=1; i<images_.size(); ++i) {
      vil_gauss_reduce(images_[i-1],images_[i], work_img);
    }
  }

protected:

  //: construct the pyramid. Assumes that the base level (0) has been populated already.
  void build_pyramid(unsigned int min_dimsize = 50)
  {
    vil_image_view<T> &hires_img = images_[0];
    unsigned int half_i = (hires_img.ni()+1)/2;
    unsigned int half_j = (hires_img.nj()+1)/2;
    unsigned int nplanes = hires_img.nplanes();

    while ( (half_j > min_dimsize) && (half_i > min_dimsize) ) {
      vil_image_view<T> lores_img(half_i,half_j,nplanes);     
      images_.push_back(lores_img);
      half_i = (half_i + 1)/2;
      half_j = (half_j + 1)/2;
    }
  }
  vcl_vector<vil_image_view<T> > images_;
};


#endif

