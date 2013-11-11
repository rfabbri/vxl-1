// This is brcv/seg/dbdet/vis/dbdet_livewire_tool.cxx
//:
// \file

#include <dbdet/lvwr/dbdet_lvwr_fit.h>
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>

#include <vcl_ctime.h>

#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

//: Constructor 
dbdet_lvwr_fit::dbdet_lvwr_fit(dbdet_lvwr_params iparams, osl_canny_ox_params params) 
{
  intsciss_.set_params(iparams);
  intsciss_.set_canny_params(params);
  contour_.clear();
}

//: set image and polygon
//  if crop is true crops the input image and moves the polygon
//  if random is set, choose initial seed point randomly as one of polygon points,
//  otherwise use the first pointbool 
bool
dbdet_lvwr_fit::initialize(vsol_polygon_2d_sptr poly, vil_image_resource_sptr image, bool crop, bool random)
{
  poly_ = new vsol_polygon_2d(*poly);
  
  if (!poly || !poly_) {
    vcl_cout << "Polygon is not set!\n";
    return false;
  }

  if (poly_->size() <= 0) {
    vcl_cout << "Empty polygon!\n";
    return false;
  }

  vil_image_resource_sptr image_sptr;
  if (crop) {
    poly_->compute_bounding_box();
  
    double lenx, leny;
    topx_ = poly_->get_min_x();
    topy_ = poly_->get_min_y();
    lenx = (poly_->get_max_x() - topx_);
    leny = (poly_->get_max_y() - topy_);
    double marginx = lenx<20?20:lenx, marginy = leny<20?20:leny;

    int w = image->ni(), h = image->nj();
    topx_ = (topx_-marginx) <= 0 ? 0.0 : topx_ - marginx;
    topy_ = (topy_-marginy) <= 0 ? 0.0 : topy_ - marginy;
    lenx = (topx_+lenx+2*marginx) > w ? (w-topx_-1) : (lenx+2*marginx);
    leny = (topy_+leny+2*marginy) > h ? (h-topy_-1) : (leny+2*marginy);
    image_sptr = vil_crop(image,
                     (unsigned int)vcl_floor(topx_+0.5),
                     (unsigned int)vcl_floor(lenx+0.5),
                     (unsigned int)vcl_floor(topy_+0.5),
                     (unsigned int)vcl_floor(leny+0.5));
  } else {
    image_sptr = image;
    topx_ = 0;
    topy_ = 0;
  }

  if (random) {
    srand(time(NULL));
    double r = ( (double)rand()/((double)(RAND_MAX)+(double)(1)) ); 
    double x = (r * poly_->size()); 
    start_index_ = (unsigned int)vcl_floor(x); 
  } else 
    start_index_ = 0;

  int cx = (int)vcl_floor(poly_->get_min_x()-topx_+0.5);
  int cy = (int)vcl_floor(poly_->get_min_y()-topy_+0.5);
  int cmx = (int)vcl_floor(poly_->get_max_x()-topx_+0.5);
  int cmy = (int)vcl_floor(poly_->get_max_y()-topy_+0.5);
  if (cx <= 6 || cy <= 6 || cmx >= (int)(image_sptr->ni()-6) || cmy >= (int)(image_sptr->nj()-6)) {
    vcl_cout << "Polygon is out of exceptable image area, skipping!\n"; 
    return false;
  }

  seed_x_ = (int)vcl_floor(poly_->vertex(start_index_)->x()-topx_+0.5);
  seed_y_ = (int)vcl_floor(poly_->vertex(start_index_)->y()-topy_+0.5);

  if (!(seed_x_ >= 0 && seed_x_ < (int)image_sptr->ni() && seed_y_ >= 0 && seed_y_ < (int)image_sptr->nj())) {
    vcl_cout << "Polygons first point is not a valid seed point on image!!\n"; 
    return false;
  }
  
  vil_image_resource_sptr grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image_sptr->get_view() ) );
  vil_image_view<float> float_image = brip_vil_float_ops::convert_to_float(*grey_image_sptr);
  if (intsciss_.params_.use_given_image) {
    vil_image_view<double> double_img;
    vil_convert_stretch_range(float_image, double_img, 0.0f, 1.0f);
    vil_convert_cast(double_img, float_image);
  }
  vil1_memory_image_of<float> img = vil1_from_vil_image_view(float_image);

  intsciss_.compute(img, seed_x_, seed_y_);
  vcl_cout << "Paths are computed, with seed x: " << seed_x_ << " seed y: " << seed_y_ << "...\n";

  contour_.clear();
  return true;
}

//: use input polygons vertices as free mouse points to trace a contour on the image
vsol_polygon_2d_sptr
dbdet_lvwr_fit::fit(bool smooth_flag, float sigma)
{
  unsigned int end_index = poly_->size()+start_index_;
  for (unsigned int ind = start_index_; ind<end_index; ind++) {
    int mouse_x = (int)vcl_floor(poly_->vertex(ind%poly_->size())->x()-topx_+0.5);
    int mouse_y = (int)vcl_floor(poly_->vertex(ind%poly_->size())->y()-topy_+0.5);

    cor_.clear();

    bool out = intsciss_.get_path(mouse_x, mouse_y, cor_);

    if (cor_.size() > 1) {
      if (!out) {

        continue;

      } else {  // mouse pointer is out of the rectangle
        // find a candidate to be a seed
        float cost1, cost2, cost_dif, cost;
        cost1 = intsciss_.get_global_cost(cor_[0].second, cor_[0].first);
        cost_dif = 0;
        for (unsigned int i = 1; i < cor_.size(); i++) {
          cost2 = intsciss_.get_global_cost(cor_[i].second, cor_[i].first);
          cost_dif += cost1-cost2;
          cost1 = cost2;
        }
        cost_dif /= cor_.size(); // average cost difference

        last_xx = cor_[1].second;
        last_yy = cor_[1].first;
        last_x = seed_x_;
        last_y = seed_y_;
        cost1 = intsciss_.get_global_cost(cor_[0].second, cor_[0].first);
        for (unsigned int i = 1; i < cor_.size(); i++) {

          cost2 = intsciss_.get_global_cost(cor_[i].second, cor_[i].first);
          cost = cost1-cost2;
          if (cost <= cost_dif) {
            last_x = cor_[i].second;
            last_y = cor_[i].first;
            break;
          }
          cost1 = cost2;
        }

        if ( vcl_abs(float(last_x - seed_x_)) > 2 || vcl_abs(float(last_y - seed_y_)) > 2) {
          seed_x_ = last_x;
          seed_y_ = last_y;
        } else {
          seed_x_ = last_xx;
          seed_y_ = last_yy;
        }

        intsciss_.get_path(seed_x_, seed_y_, cor_);

        if (cor_.size() != 0) {
          vcl_vector<vcl_pair<int, int> >::iterator p;
          for (p = cor_.end()-1; p != cor_.begin()-1; p--) {
            contour_.push_back(new vsol_point_2d(double(p->second+topx_), double(p->first+topy_)));
          }
        }

        intsciss_.compute_directions(seed_x_, seed_y_);
      }
    }
    
  }
  //: push back the last portion if any
  if (cor_.size() > 0) {
    vcl_vector<vcl_pair<int, int> >::iterator p;
    for (p = cor_.end()-1; p != cor_.begin()-1; p--) {
      contour_.push_back(new vsol_point_2d(double(p->second+topx_), double(p->first+topy_)));
    }
  }

  if (smooth_flag) {
    smooth(sigma);
  }
  if (contour_.size() >= 3)
    return new vsol_polygon_2d(contour_);
  else 
    return new vsol_polygon_2d();
}

//: smooth the output contour n times
bool 
dbdet_lvwr_fit::smooth(float /*sigma*/)
{
  vcl_vector<vgl_point_2d<double> > curve;
  curve.clear();
    
  for (unsigned i = 0; i<contour_.size(); i++) {
    curve.push_back(vgl_point_2d<double>(contour_[i]->x(), 
                                         contour_[i]->y()));
  }

  bdgl_curve_algs::smooth_curve(curve, 1.0);
  
  contour_.clear();
  curve.erase(curve.begin());
  curve.erase(curve.begin());
  curve.erase(curve.begin());

  curve.erase(curve.end()-1);
  curve.erase(curve.end()-1);
  curve.erase(curve.end()-1);
   
  for (unsigned i = 0; i<curve.size(); i++) {
    contour_.push_back(new vsol_point_2d(curve[i].x(), curve[i].y()));
  }

  return true;
}
