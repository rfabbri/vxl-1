#include "dbru_rcor.h"

#include <vsol/vsol_point_2d.h>
#include <btol/btol_face_algs.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>

#include <dbsol/dbsol_interp_curve_2d.h>

#include <dbskr/dbskr_scurve.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
//#include <dbinfo/dbinfo_intensity_feature.h>
//#include <dbinfo/dbinfo_gradient_feature.h>
//#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>

#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <bsol/bsol_algs.h>
#include <vcl_algorithm.h> 

#include <vil/vil_convert.h>
#include <vil/vil_new.h>

#define LARGE_INT (1000000)
#define SMALL (1E-5)

//: either sillhouette or shock matching correspondence should be set after an instance of this class is
//  constructred
dbru_rcor::dbru_rcor(dbinfo_observation_sptr obs1, dbinfo_observation_sptr obs2, bool save_histograms, bool save_float) : sil_cor_(0), sm_cor_(0) {
  halt_ = false;
  save_histograms_ = save_histograms;
  save_float_ = save_float;
  
  obs1_ = obs1;
  obs2_ = obs2;

  dbinfo_region_geometry_sptr geo1 = obs1->geometry();
  dbinfo_region_geometry_sptr geo2 = obs2->geometry();

  max1_x_ = 0;
  max1_y_ = 0;
  min1_x_ = LARGE_INT;
  min1_y_ = LARGE_INT;
  cnt2_ = 0;

  //: for now assume the geometry has only one poly
  assert(geo1->n_polys() == 1 && geo2->n_polys() == 1);

  p1_ = bsol_algs::vgl_from_poly(geo1->poly(0));
  p2_ = bsol_algs::vgl_from_poly(geo2->poly(0));

  //: determine the size of the region first
  for (unsigned i = 0; i<geo1->n_polys(); i++) {
      vsol_polygon_2d_sptr p = geo1->poly(i);
      p->compute_bounding_box();
      int min_x = p->get_min_x()-5 < 0.0f ? 0 : (int)vcl_floor(p->get_min_x()-5.0f);
      int min_y = p->get_min_y()-5 < 0.0f ? 0 : (int)vcl_floor(p->get_min_y()-5.0f);
      int max_x = (int)vcl_ceil(p->get_max_x()+5.0f);
      int max_y = (int)vcl_ceil(p->get_max_y()+5.0f);
      if (min_x < min1_x_)
        min1_x_ = min_x;
      if (min_y < min1_y_)
        min1_y_ = min_y;
      if (max_x > max1_x_)
        max1_x_ = max_x;
      if (max_y > max1_y_)
        max1_y_ = max_y;
  }

  upper_x_ = max1_x_-min1_x_+1; 
  upper_y_ = max1_y_-min1_y_+1; 
  for (int i = 0; i<upper_x_; i++) {
    vcl_vector<int> tmp(upper_y_, -1);
    region1_.push_back(tmp);
  }

  // get the points from the observation to be consistent with it
  for (unsigned i = 0; i< geo1->size(); i++) {
    vgl_point_2d<float> p = geo1->point(i);
    region1_[(int)p.x()-min1_x_][(int)p.y()-min1_y_] = i;
  }
  cnt2_ = geo1->size();

  if (!cnt2_)
  {
    vcl_cout << "No pixels in region 1\n";
    halt_ = true;
  }

  max2_x_ = 0;
  max2_y_ = 0;
  min2_x_ = LARGE_INT;
  min2_y_ = LARGE_INT;
  int n2 = 0;

  //: determine the size of the region first
  for (unsigned i = 0; i<geo2->n_polys(); i++) {
      vsol_polygon_2d_sptr p = geo2->poly(i);
      p->compute_bounding_box();
      int min_x = p->get_min_x()-5 < 0.0f ? 0 : (int)vcl_floor(p->get_min_x()-5.0f);
      int min_y = p->get_min_y()-5 < 0.0f ? 0 : (int)vcl_floor(p->get_min_y()-5.0f);
      int max_x = (int)vcl_ceil(p->get_max_x()+5.0f);
      int max_y = (int)vcl_ceil(p->get_max_y()+5.0f);
      if (min_x < min2_x_)
        min2_x_ = min_x;
      if (min_y < min2_y_)
        min2_y_ = min_y;
      if (max_x > max2_x_)
        max2_x_ = max_x;
      if (max_y > max2_y_)
        max2_y_ = max_y;
  }

  upper2_x_ = max2_x_-min2_x_+1;
  upper2_y_ = max2_y_-min2_y_+1;
  for (int i = 0; i<upper2_x_; i++) {
    vcl_vector<int> tmp(upper2_y_, -1);
    region2_.push_back(tmp);
  }

  // get the points from the observation to be consistent with it
  for (unsigned i = 0; i< geo2->size(); i++) {
    vgl_point_2d<float> p = geo2->point(i);
    region2_[(int)p.x()-min2_x_][(int)p.y()-min2_y_] = i;
  }
  n2 = geo2->size();

  if (!n2)
  {
    vcl_cout << "No pixels in region 2\n";
    halt_ = true;
  }
  
  //: prepare our output which is a map from x, y in region 1 to x2, y2 in region2
  region1_map_output_.resize(upper_x_, upper_y_);
  vgl_point_2d<int> p(-1, -1);
  region1_map_output_.fill(p);

  //: prepare our output which is a map from x, y in region 1 to x2, y2 in region2
  if (save_float_) {
    region1_map_output_float_.resize(upper_x_, upper_y_);
    vgl_point_2d<float> p2(-1.0f, -1.0f);
    region1_map_output_float_.fill(p2);
  }
}

// this function should be called before generating correspondences using "line intersections" 
// algorithms, other algorithms are not using this data structure
void dbru_rcor::initialize_region1_histograms() {
  region1_histograms_.resize(upper_x_, upper_y_);
  vcl_vector< vcl_pair< vgl_point_2d<int>, int > > tmp;
  region1_histograms_.fill(tmp);

  if (save_float_) {
    region1_histograms_float_.resize(upper_x_, upper_y_);
    vcl_vector< vcl_pair< vgl_point_2d<float>, int > > tmp2;
    region1_histograms_float_.fill(tmp2);
  }
}

// this method should be called if save float option is turned on 
// after an instance is constructed 
void dbru_rcor::initialize_float_map() {
  region1_map_output_float_.resize(upper_x_, upper_y_);
  vgl_point_2d<float> p2(-1.0f, -1.0f);
  region1_map_output_float_.fill(p2);
}

void dbru_rcor::clear_region1_histograms() {
  region1_histograms_.clear();
  region1_histograms_float_.clear();
}

//: clear current region correspondence to allow for matching using the same sillhoette correspondence
void dbru_rcor::clear_region_correspondence() {
  correspondences_.clear(); 
  vgl_point_2d<int> p(-1, -1);
  region1_map_output_.fill(p);

  if (save_float_) {
    vgl_point_2d<float> p2(-1.0f, -1.0f);
    region1_map_output_float_.fill(p2);
  }
}

//: get the image that shows the pixels used in region1/observation1
vil_image_resource_sptr dbru_rcor::get_used_pixels1() {

  dbinfo_region_geometry_sptr geo = obs1_->geometry();
  vsol_polygon_2d_sptr poly = geo->poly(0);
  poly->compute_bounding_box();
  //unused int s = poly->size();
  int mx = (int)vcl_floor(poly->get_min_x()+0.5);
  int my = (int)vcl_floor(poly->get_min_y()+0.5);
  int maxx = (int)vcl_floor(poly->get_max_x()+0.5);
  int maxy = (int)vcl_floor(poly->get_max_y()+0.5);
  int w = maxx-mx+10;
  int h = maxy-my+10;
  
  vil_image_view<float> image_out(w,h,1);

  for (int y = 0; y<h; y++ ) 
    for (int x = 0; x<w; x++)
      image_out(x,y) = 255;

  dbinfo_feature_data_base_sptr d0 = (obs1_->features())[0]->data();
  assert(d0->format() == DBINFO_INTENSITY_FEATURE);
  dbinfo_feature_data<vbl_array_1d<float> >* cd0 = dbinfo_feature_data<vbl_array_1d<float> >::ptr(d0);
  vbl_array_1d<float>& v0 = cd0->single_data();       

  for(unsigned k = 0; k<correspondences_.size(); ++k)
  {
    int region1_id = correspondences_[k].first;
    float vv0 = v0[region1_id];
    vgl_point_2d<float> coord = geo->point(region1_id);
    int yy = (int)(coord.y()-my+5);
    int xx = (int)(coord.x()-mx+5);
    if (xx < 0 || yy < 0 || xx >= w || yy >= h)
      continue;
    image_out(xx,yy) = vv0;
  }

  vil_image_view<vxl_byte> output_img;
  vil_convert_stretch_range(image_out, output_img);
  vil_image_resource_sptr output_sptr = vil_new_image_resource_of_view(output_img);
  return output_sptr;
}

//: get the image that shows the pixels used in region2/observation2
vil_image_resource_sptr dbru_rcor::get_used_pixels2() {

  dbinfo_region_geometry_sptr geo = obs2_->geometry();
  vsol_polygon_2d_sptr poly = geo->poly(0);
  poly->compute_bounding_box();
  //unused int s = poly->size();
  int mx = (int)vcl_floor(poly->get_min_x()+0.5);
  int my = (int)vcl_floor(poly->get_min_y()+0.5);
  int maxx = (int)vcl_floor(poly->get_max_x()+0.5);
  int maxy = (int)vcl_floor(poly->get_max_y()+0.5);
  int w = maxx-mx+10;
  int h = maxy-my+10;
  
  vil_image_view<float> image_out(w,h,1);

  for (int y = 0; y<h; y++ ) 
    for (int x = 0; x<w; x++)
      image_out(x,y) = 255;

  dbinfo_feature_data_base_sptr d0 = (obs2_->features())[0]->data();
  assert(d0->format() == DBINFO_INTENSITY_FEATURE);
  dbinfo_feature_data<vbl_array_1d<float> >* cd0 = dbinfo_feature_data<vbl_array_1d<float> >::ptr(d0);
  vbl_array_1d<float>& v0 = cd0->single_data();       

  for(unsigned k = 0; k<correspondences_.size(); ++k)
  {
    int region2_id = correspondences_[k].second;
    float vv0 = v0[region2_id];
    vgl_point_2d<float> coord = geo->point(region2_id);
    int yy = (int)(coord.y()-my+5);
    int xx = (int)(coord.x()-mx+5);
    if (xx < 0 || yy < 0 || xx >= w || yy >= h)
      continue;
    image_out(xx,yy) = vv0;
  }

  vil_image_view<vxl_byte> output_img;
  vil_convert_stretch_range(image_out, output_img);
  vil_image_resource_sptr output_sptr = vil_new_image_resource_of_view(output_img);
  return output_sptr;
}

//: get the image that puts corresponding region2 pixel intensities on top of region1 pixels
vil_image_resource_sptr dbru_rcor::get_appearance2_on_pixels1() {

  dbinfo_region_geometry_sptr geo1 = obs1_->geometry();
  vsol_polygon_2d_sptr poly1 = geo1->poly(0);
  poly1->compute_bounding_box();
  //unused int s = poly1->size();
  int mx = (int)vcl_floor(poly1->get_min_x()+0.5);
  int my = (int)vcl_floor(poly1->get_min_y()+0.5);
  int maxx = (int)vcl_floor(poly1->get_max_x()+0.5);
  int maxy = (int)vcl_floor(poly1->get_max_y()+0.5);
  int w = maxx-mx+10;
  int h = maxy-my+10;
  
  vil_image_view<float> image_out(w,h,1);

  for (int y = 0; y<h; y++ ) 
    for (int x = 0; x<w; x++)
      image_out(x,y) = 255;

  dbinfo_feature_data_base_sptr d2 = (obs2_->features())[0]->data();
  assert(d2->format() == DBINFO_INTENSITY_FEATURE);
  dbinfo_feature_data<vbl_array_1d<float> >* cd2 = dbinfo_feature_data<vbl_array_1d<float> >::ptr(d2);
  vbl_array_1d<float>& v2 = cd2->single_data();       

  for(unsigned k = 0; k<correspondences_.size(); ++k)
  {
    int region1_id = correspondences_[k].first;
    int region2_id = correspondences_[k].second;
    
    float vv2 = v2[region2_id];
    vgl_point_2d<float> coord = geo1->point(region1_id);
    int yy = (int)(coord.y()-my+5);
    int xx = (int)(coord.x()-mx+5);
    if (xx < 0 || yy < 0 || xx >= w || yy >= h)
      continue;
    image_out(xx,yy) = vv2;
  }

  vil_image_view<vxl_byte> output_img;
  vil_convert_stretch_range(image_out, output_img);
  vil_image_resource_sptr output_sptr = vil_new_image_resource_of_view(output_img);
  return output_sptr;
}
