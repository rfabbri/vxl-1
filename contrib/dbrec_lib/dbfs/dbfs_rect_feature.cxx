//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   Oct 04, 2009
//
#include "dbfs_rect_feature.h"

#include <vil/vil_new.h>

dbfs_rect_feature::dbfs_rect_feature(const vil_image_view<vxl_byte>& img, int i, int j, int w, int h) : w_(w), h_(h), f_(w*h, vxl_byte(0)) 
{

  for (int ii = i; ii < i+w; ii++)
    for (int jj = j; jj < j+h; jj++) {
      int k = (ii-i)*w + (jj-j);
      f_[k] = img(ii,jj);
    }
}

float dbfs_rect_feature::measure_foreground(const vil_image_view<float>& img_f, int i, int j)
{
  float sum = 0.0f;
  for (int ii = i; ii < i+w_; ii++)
    for (int jj = j; jj < j+h_; jj++) {
      sum += img_f(ii, jj);
    }
  return sum/(w_*h_);
}

vil_image_resource_sptr dbfs_rect_feature::create_patch()
{
  vil_image_view<vxl_byte> img(w_,h_);
  for (unsigned k = 0; k < f_.size(); k++) {
    int ii = k/w_;
    int jj = k%w_;
    img(ii, jj) = f_[k];
  }
  vil_image_resource_sptr img_r = vil_new_image_resource_of_view(img);
  return img_r;
}

void dbfs_rect_feature_set::populate(const vil_image_view<vxl_byte>& img, const vil_image_view<float>& img_f, int w, int h, float fg_thres)
{
  vcl_cout << "In populate features!\n";
  int si = 0; int ei = img.ni()-w;
  int sj = 0; int ej = img.nj()-h;

  for (int i = si; i < ei; i++) 
    for (int j = sj; j < ej; j++) {
      dbfs_rect_feature_sptr f = new dbfs_rect_feature(img, i, j, w, h);
      float fg_m = f->measure_foreground(img_f, i, j);
      vcl_cout << "fg_m is: " << fg_m << " threshold is " << fg_thres << vcl_endl;
      if (fg_m > fg_thres)
        this->add_feature(f);
    }
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbfs_rect_feature_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbfs_rect_feature_set const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbfs_rect_feature_set &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbfs_rect_feature_set* ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const dbfs_rect_feature_set* &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}
