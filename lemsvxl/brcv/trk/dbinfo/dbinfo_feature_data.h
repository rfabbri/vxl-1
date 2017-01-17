// This is brl/bseg/dbinfo/dbinfo_feature_data.h
#ifndef dbinfo_feature_data_h_
#define dbinfo_feature_data_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A templated storage class for feature data
// \author J.L. Mundy March 20, 2005
//
// \verbatim
//  Modifications <none>
// \endverbatim

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <dbinfo/dbinfo_feature_data_base.h>
template <class T>
class dbinfo_feature_data : public dbinfo_feature_data_base
{
 protected:
  //:The actual stored data.  Vector storage to support Markov representations.
  //for now, just a single sample
  vcl_vector<T> feature_data_;


 public:
  //the default constructor - not really usable
  dbinfo_feature_data():dbinfo_feature_data_base(){}

  //the constructor for empty data - data is added incrementally
  dbinfo_feature_data(const vil_pixel_format pix_format,
                      const unsigned nplanes,
                      const dbinfo_feature_format format):
    dbinfo_feature_data_base(pix_format, nplanes, format){}

  //:  constructor where data is added sequentially to a fixed size store
  dbinfo_feature_data(const vil_pixel_format pix_format,
                      const unsigned nplanes,
                      const dbinfo_feature_format format,
                      const unsigned size);

  //: constructor from data
  dbinfo_feature_data(const vil_pixel_format pix_format,
                      const unsigned nplanes,
                      const dbinfo_feature_format format,
                      vcl_vector<T> const& data);

  //:  copy constructor
  dbinfo_feature_data(const dbinfo_feature_data<T> & rhs);

  //: Construct from abstract base
  dbinfo_feature_data(const dbinfo_feature_data_base& rhs);

  //: Copy constructor from abstract base sptr
  dbinfo_feature_data(const dbinfo_feature_data_base_sptr& rhs)
    { operator=(rhs);}

  //: Pointer cast from abstract base sptr
  static dbinfo_feature_data* ptr(dbinfo_feature_data_base_sptr& rhs)
    {return static_cast<dbinfo_feature_data<T>* >(rhs.ptr());}

  //:distructor
  ~dbinfo_feature_data() {}

  //: the current size of the store
  inline virtual unsigned size() const {return feature_data_.size();}

  //:add data to the fixed size store
  inline void set_sample(const unsigned frame, T const& data,
                         const float w = 1.0)
  {dbinfo_feature_data_base::set_sample(frame, w);
  feature_data_[index(frame)]=data;}

  inline void set_single_sample(T const& data)
    { feature_data_.clear(); feature_data_.push_back(data);}


  //:add data to a variable size store
  void add_sample(const unsigned frame, T const& data, const float w = 1.0){assert(!fixed_storage_);
  dbinfo_feature_data_base::add_sample(frame, w);
  feature_data_.push_back(data);}
  
  //:get data
  inline  T& data(unsigned frame)
    {assert(index(frame)<size());return feature_data_[index(frame)];}

  inline vcl_vector<T> data(){return feature_data_;}

  inline T& single_data(){assert(size()==1);return feature_data_[0];}
//: This operator de-references an image_view sptr, returning an empty view if the pointer is null.
  inline const dbinfo_feature_data<T>& operator=(const dbinfo_feature_data_base_sptr& rhs)
    {
      if (!rhs)
        *this = *(new dbinfo_feature_data<T>());
      else
        *this = *rhs;
      return *this;
    }

  virtual dbinfo_feature_data_base* clone() const
    {return new dbinfo_feature_data<T>(*this);}

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const;

  // Binary I/O------------------------------------------------------------------

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return vcl_string("dbinfo_feature_data");}

  bool is_class(vcl_string const& cls) const
    { return cls==is_a();}

   //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

};
template <class T>
inline void vsl_b_read(vsl_b_istream &is, dbinfo_feature_data<T> & v)
{v.b_read(is);}

template <class T>
inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_feature_data<T> & v)
{v.b_write(os);}


template <class T>
inline void vsl_print_summary(vcl_ostream& os,
                              const dbinfo_feature_data<T> & b)
{os << b;}

template<class T> 
inline void vsl_b_write(vsl_b_ostream &os, dbinfo_feature_data<T> const* fd)
{
  //Don't allow writing for null feature data
  assert(fd);
  const dbinfo_feature_data<T>& cfd = *fd;
  vsl_b_write(os, cfd);
}

//: Binary load dbinfo_feature_data from stream.
template<class T> 
inline void vsl_b_read(vsl_b_istream &is, dbinfo_feature_data<T>* &fd)
{
  delete fd;
  fd = new dbinfo_feature_data<T>();
  vsl_b_read(is, *fd);
}
//: Write a generic pointer to feature data to the serial stream
inline void vsl_b_write(vsl_b_ostream &os, dbinfo_feature_data_base const* fd)
{
  assert(fd);
  vsl_b_write(os, fd->version());
  dbinfo_feature_format format = fd->format();
  vsl_b_write(os, format);
  fd->b_write(os);
}

//: Read a generic pointer to feature data from the serial stream
//A bit ugly but simple for now. vsl_binary_loader seems to croak on
//templated sub classes.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_feature_data_base* &fd)
{
  delete fd;
  int fmt = 0;
  short version;
  vsl_b_read(is, version);
  if(version!=1)
    {
      fd = (dbinfo_feature_data_base*)0;
      return;
    }
  vsl_b_read(is, fmt);
  dbinfo_feature_format format = (dbinfo_feature_format)fmt;
  switch(format)
    {
    case DBINFO_FEATURE_FORMAT_UNKNOWN: 
      fd =  (dbinfo_feature_data_base*)0;
      return;
    case DBINFO_INTENSITY_FEATURE:
      fd = new dbinfo_feature_data<vbl_array_1d<float> >();
      fd->b_read(is);
      return;
    case DBINFO_GRADIENT_FEATURE:
      fd = new dbinfo_feature_data<vbl_array_2d<float> >();
      fd->b_read(is);
      return;
    case DBINFO_IHS_FEATURE:
      fd = (dbinfo_feature_data_base*)0;
      return;
    default:
      fd = (dbinfo_feature_data_base*)0;
    }
}

template <class T>
vcl_ostream &operator<<(vcl_ostream &, dbinfo_feature_data<T> const&);
#if 0
template <class T>
vcl_ostream &operator<<(vcl_ostream &, dbinfo_feature_data<T> const*);
#endif


#endif // dbinfo_feature_data_h_
