#ifndef imgr_view_data_base_h_
#define imgr_view_data_base_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for view data, acts as a kind of memory chunk
// \author J. L. Mundy
// \date Mar. 08, 2005
// The data is typically too large to be deep copied so
// data is passed by pointer to each copy of a 3-d image view. The 
// storage is a vector of 2-d image view pointers.  This approach
// uses the existing machinery of vil_image_view without the need to manage
// raw memory.  vil_image_view_base is used to abstract away template type
// dependency.
//
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view_base.h>

class imgr_view_data_base : public vbl_ref_count
{
 protected:
  vcl_vector<vil_image_view_base_sptr > views_2d_;
 public:
  //:constructors
  imgr_view_data_base(){}
  imgr_view_data_base(vcl_vector<vil_image_view_base_sptr> const& views)
    : views_2d_(views){}

  virtual ~imgr_view_data_base(){}

  //:accessors
  void add_view_2d(vil_image_view_base_sptr const& view_2d)
    {views_2d_.push_back(view_2d);}

  const vil_image_view_base_sptr& view_2d(unsigned k) const
    {return views_2d_[k];}

  unsigned size() const {return views_2d_.size();}
};

#include <imgr/imgr_view_data_base_sptr.h>
#endif // imgr_view_data_base_h_
