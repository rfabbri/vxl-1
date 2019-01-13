//:
// \file
// \brief Class that ..
//
// \author Based on original code by  Ozge C Ozcanli (@lems.brown.edu)
// \date 10/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
// see borld/algo/borld_image_desc_parser.h for xml read/write utilities
//

#if !defined(_DBORL_IMAGE_BBOX_DESCRIPTION_H)
#define _DBORL_IMAGE_BBOX_DESCRIPTION_H

#include <borld/borld_image_data_description_base.h>

#include <vsol/vsol_box_2d_sptr.h>

class borld_image_bbox_description : public borld_image_data_description_base
{
public:
  std::map<std::string, std::vector<vsol_box_2d_sptr> > data_;

  borld_image_bbox_description() {}
  ~borld_image_bbox_description() { data_.clear(); }

  virtual unsigned get_type() { return borld_image_data_description_type::bounding_box; }

  void add_box(std::string cat, vsol_box_2d_sptr b);
  bool category_exists(std::string cat) { return data_.find(cat) != data_.end(); }

  //: CAUTION: assumes that cat exists!! check with category_exists() before using
  std::vector<vsol_box_2d_sptr>& get_box_vector(std::string cat);

  std::map<std::string, std::vector<vsol_box_2d_sptr> >& get_category_map() { return data_; }

  //: update version number whenever binary reader/writer is updated
  virtual unsigned version();
  virtual void b_read();
  virtual void b_write();
  virtual void write_xml(std::ostream& os);

  virtual borld_image_bbox_description* cast_to_image_bbox_description() { return this; }
  virtual borld_image_mask_description* cast_to_image_mask_description() { return 0; }
  virtual borld_image_polygon_description* cast_to_image_polygon_description() { return 0; }
};

#endif  //_DBORL_IMAGE_BBOX_DESCRIPTION_H
