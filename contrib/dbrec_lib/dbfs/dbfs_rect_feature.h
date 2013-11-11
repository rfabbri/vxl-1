//:
// \file
// \brief Implementation of rectangular patch features as described in "Distinctive and compact features", by Ayelet Akselrod-Ballin, Shimon Ullman
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   Oct 04, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//
//
//


#if !defined(_dbfs_rect_feature_h)
#define _dbfs_rect_feature_h

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include "dbfs_rect_feature_sptr.h"

class dbfs_rect_feature : public vbl_ref_count
{
public:
  dbfs_rect_feature(const vil_image_view<vxl_byte>& img, int i, int j, int w, int h);
  
  vil_image_resource_sptr create_patch();
  float measure_foreground(const vil_image_view<float>& img_f, int i, int j);
   
protected:
  int w_;
  int h_;
  vcl_vector<vxl_byte> f_;
};

class dbfs_rect_feature_set : public vbl_ref_count
{
public:
  dbfs_rect_feature_set() {}
  void populate(const vil_image_view<vxl_byte>& img, const vil_image_view<float>& img_f, int w, int h, float fg_thres = 0.5f);
  void add_feature(dbfs_rect_feature_sptr f) { s_.push_back(f); }
  unsigned size() { return s_.size(); };
  dbfs_rect_feature_sptr get_feature(unsigned i) { return s_[i]; }

protected:
  vcl_vector<dbfs_rect_feature_sptr> s_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use dbfs_rect_feature_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbfs_rect_feature_set const &ph);
void vsl_b_read(vsl_b_istream & is, dbfs_rect_feature_set &ph);
void vsl_b_read(vsl_b_istream& is, dbfs_rect_feature_set* ph);
void vsl_b_write(vsl_b_ostream& os, const dbfs_rect_feature_set* &ph);

#endif  //_dbfs_rect_feature_h
