// This is biotree/resc/imgr/imgr_bounded_image_view_3d.h
#ifndef imgr_bounded_image_view_3d_h_
#define imgr_bounded_image_view_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A 3-d image view of a set of bounded images 
// \author J.L. Mundy (after Ian Scott's vil3d design)
//
// \verbatim
//  Modifications <none>
// \endverbatim
//  This view provides a set of 2-d dbil_bounded_image_view(s). Each 2-d view
//  supports pixel access the full image space,  even though only data
//  within the bounds is actually available
//  
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_smart_ptr.h>
#include <vgl/vgl_box_3d.h>
#include <vsl/vsl_binary_io.h>
#include <dbil/dbil_bounded_image_view.h>
#include <imgr/imgr_view_data_base_sptr.h>
#include <imgr/imgr_image_view_3d_base.h>

template <class T>
class imgr_bounded_image_view_3d : public imgr_image_view_3d_base
{
 protected:
  //:This is a reference counted set of 2-d views. Copy just passes the pointer
  imgr_view_data_base_sptr view_data_2d_;

  //:The 3-d box used to generate the 2-d views
  vgl_box_3d<double> bounds_;

  //: get the view data
  imgr_view_data_base_sptr view_data(){return view_data_2d_;} 
  
 public:
   //the default constructor doesn't make sense so should not be used
   imgr_bounded_image_view_3d(){};

  //: full constructor
  imgr_bounded_image_view_3d(vcl_vector<dbil_bounded_image_view<T>* > const& views,
                             vgl_box_3d<double> const& bounds);

  //:  constructor where images are added later
  imgr_bounded_image_view_3d(vgl_box_3d<double> const& bounds,
                             unsigned ni, unsigned nj, unsigned nk,
                             unsigned nplanes
                             );

  //:  copy constructor, just assigns a new instance to the same view data
  imgr_bounded_image_view_3d(const imgr_bounded_image_view_3d<T> & rhs);


  //: Return the 3-d bounds used to generate the view_3d
  vgl_box_3d<double> bounds() {return bounds_;}

  //: the number of images in the 3-d view, must be equal to view_data size
  virtual unsigned nk()  const
    {assert(nk_==view_data_2d_->size());
    return nk_;}

  virtual vil_pixel_format pixel_format() const 
    { return vil_pixel_format_of(T()); }

   //:add a bounded_image_view to the view_3d
  inline void add_view_2d(const dbil_bounded_image_view<T>* view_2d_ptr)
    {view_data_2d_->add_view_2d((vil_image_view<T>*)view_2d_ptr);}
  

  //:get a bounded_image_view
  inline  dbil_bounded_image_view<T> view_2d(unsigned k)
    {return view_data_2d_->view_2d(k);}

  //:virtual distructor
  virtual ~imgr_bounded_image_view_3d() {}

  //: Construct from abstract base
  imgr_bounded_image_view_3d(const imgr_image_view_3d_base& rhs);

  //: Construct from abstract base sptr
  imgr_bounded_image_view_3d(const imgr_image_view_3d_base_sptr& rhs)
  { operator=(rhs);}

  //: This operator de-references an image_view sptr, returning an empty view if the pointer is null.
  inline const imgr_bounded_image_view_3d<T>& operator=(const imgr_image_view_3d_base_sptr& rhs)
  {
    if (!rhs)
      *this = *(new imgr_bounded_image_view_3d<T>(vgl_box_3d<double>(), 0, 0, 0, 0));
    else
      *this = *rhs;
    return *this;
  }

  short version() const;

  void  b_write(vsl_b_ostream &os) const;

  void b_read(vsl_b_istream &is);
};

#endif // imgr_bounded_image_view_3d_h_
