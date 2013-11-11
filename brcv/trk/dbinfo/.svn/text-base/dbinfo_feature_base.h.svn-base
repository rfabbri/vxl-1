// This is brl/bseg/dbinfo/dbinfo_feature_base.h
#ifndef dbinfo_feature_base_h_
#define dbinfo_feature_base_h_
//---------------------------------------------------------------------
//:
// \file
// \brief the base class for tracking features
//
// \author
//  J.L. Mundy - March 20, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data_base.h>
//forward declare the sub-classes
class dbinfo_intensity_feature;
class dbinfo_gradient_feature;
class dbinfo_ihs_feature;

class dbinfo_feature_base : public vbl_ref_count
{

 public:

  //: Constructors/destructor
  //: default
  dbinfo_feature_base(): format_(DBINFO_FEATURE_FORMAT_UNKNOWN),
    margin_(0),  data_(0){}
  
  //: constructor from data, no current data
  dbinfo_feature_base(dbinfo_feature_data_base_sptr const& data)
    : margin_(0), data_(data){}

  //:default destructor
  ~dbinfo_feature_base(){}

  //: construct from abstract base
  dbinfo_feature_base* feature_base_new();

  //:set the stored data for the feature
  void set_data(dbinfo_feature_data_base_sptr const& data)
    {data_ = data;}

  //:get the data for the feature
  dbinfo_feature_data_base_sptr data(){return data_;}

  //:clear (delete) the stored data
  void clear_data(){data_ = (dbinfo_feature_data_base*)0;}
  
  //:abstract base methods that must be implemented on each child
  //:cache the image data for storage and mutual information calculations
  virtual bool scan(const unsigned frame,
                    vcl_vector<vgl_point_2d<unsigned> > const& points,
                    vcl_vector<bool> const& valid,
                    vil_image_resource_sptr const& image) {return false;}

  //: Reconstruct an image snippet around the feature of size n_i x n_j. Offset the feature points by (i0, j0)
  virtual vil_image_resource_sptr 
    image(vcl_vector<vgl_point_2d<float> > const& points,
          vcl_vector<bool> const& valid,
          unsigned n_i, unsigned n_j, float i0, float j0, bool background_noise = true)
    {return 0;}

  //Can't be sure that dynamic cast will work on all compilers
  //so we use this rather antiquated method
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an intensity feature, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const dbinfo_intensity_feature* 
    cast_to_intensity_feature() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a gradient feature, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const dbinfo_gradient_feature* 
    cast_to_gradient_feature() const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an ihs feature, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const dbinfo_ihs_feature* 
    cast_to_ihs_feature() const { return 0; }

  dbinfo_feature_format format(){return format_;}

  //: The margin for applying image operators needed by the feature scan
  void set_margin(unsigned margin){margin_ = margin;}

  unsigned margin() const {return margin_;}

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const;

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbinfo_feature_base";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
    { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);


 protected:
  //: feature format
  dbinfo_feature_format format_;

  //:image processing margin required by the feature
  unsigned margin_;

  //:The data stored from a previous frame
  dbinfo_feature_data_base_sptr data_;
};


inline void vsl_b_read(vsl_b_istream &is, dbinfo_feature_base & fb)
{fb.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_feature_base & fb)
{fb.b_write(os);}

//: Binary save dbinfo_feature_data pointer
inline void vsl_b_write(vsl_b_ostream &os, dbinfo_feature_base const* fb)
{
  //Don't allow writing for null feature data
  assert(fb);
  const dbinfo_feature_base& cfb = *fb;
  vsl_b_write(os, cfb);
}

//: Binary load dbinfo_feature_data pointer
inline void vsl_b_read(vsl_b_istream &is, dbinfo_feature_base* &fb)
{
  delete fb;
  fb = new dbinfo_feature_base();
  vsl_b_read(is, *fb);
}
#include <dbinfo/dbinfo_feature_base_sptr.h>

inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_feature_base const& fb)
{
  fb.print(os);
  return os;
}
#if 0
inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_feature_base const* fb)
{
  if (fb)
    os << *fb;
  else
    os << "NULL Feature \n";
  return os;
}
#endif
inline void vsl_print_summary(vcl_ostream & os, const dbinfo_feature_base* fbp)
{os << *fbp;}
  
#endif // dbinfo_feature_base_h_
