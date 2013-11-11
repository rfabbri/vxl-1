// This is brl/bseg/dbinfo/dbinfo_feature_data

#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/io/vbl_io_array_1d.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>
#include <dbinfo/dbinfo_feature_data.h>



//=======================================================================
//constructors


template<class T>
dbinfo_feature_data<T>::
dbinfo_feature_data(const vil_pixel_format pix_format,
                    const unsigned nplanes,
                    const dbinfo_feature_format format,
                    const unsigned size) 
  : dbinfo_feature_data_base(pix_format, nplanes, format, size)
{
  feature_data_.resize(size);
}

//: constructor from data
template<class T>
dbinfo_feature_data<T>::
dbinfo_feature_data(const vil_pixel_format pix_format,
                    const unsigned nplanes,
                    const dbinfo_feature_format format,
                    vcl_vector<T> const& data): 
  dbinfo_feature_data_base(pix_format, nplanes, format, data.size()),
  feature_data_(data)
{
}
  


//: copy constructor
template<class T>
dbinfo_feature_data<T>::
dbinfo_feature_data(const dbinfo_feature_data<T>& rhs)
{
  format_ = rhs.format_;
  frame_from_index_ = rhs.frame_from_index_;
  index_from_frame_ = rhs.index_from_frame_;
  weights_ =  rhs.weights_;
  fixed_storage_ = rhs.fixed_storage_;
  feature_data_ = rhs.feature_data_;
}

//:constructor from abstract base
template<class T> dbinfo_feature_data<T>::
dbinfo_feature_data(const dbinfo_feature_data_base& rhs)
  : dbinfo_feature_data_base(rhs)
{
  const dbinfo_feature_data<T>& bv = 
    static_cast<const dbinfo_feature_data<T>& >(rhs);
  feature_data_ = bv.feature_data_;
}

// Binary I/O------------------------------------------------------------------


template<class T> 
void dbinfo_feature_data<T>::b_write(vsl_b_ostream &os) const
{
  dbinfo_feature_data<T>& fd = (dbinfo_feature_data<T>&)*this;
  vsl_b_write(os, version());
  vsl_b_write(os, pix_format());
  vsl_b_write(os, nplanes());
  vsl_b_write(os, format());
  vsl_b_write(os, frame_from_index_);
  vsl_b_write(os, index_from_frame_);
  vcl_vector<T> data = fd.data();
  vsl_b_write(os, data);
}

//: Binary load from stream.
template<class T> 
void dbinfo_feature_data<T>::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        int pix_fmt;
        unsigned nplanes;
        int fmt;
        vsl_b_read(is, pix_fmt);
        vsl_b_read(is, nplanes);
        vsl_b_read(is, fmt);
        pix_format_ = (vil_pixel_format)pix_fmt;
        format_ = (dbinfo_feature_format)fmt;
        vsl_b_read(is, frame_from_index_);
        vsl_b_read(is, index_from_frame_);
        vsl_b_read(is, feature_data_);
      }
    }
}

template<class T> 
void dbinfo_feature_data<T>::
print(vcl_ostream &strm) const
{
  strm << this->is_a() << " [\n"
       << "pixel format " << pix_format_ << '\n'
       << "number of components " << nplanes_ << '\n'
       << "data format  " << format_ << '\n'
       << "]\n";
}
template <class T>
vcl_ostream &operator<<(vcl_ostream &strm, dbinfo_feature_data<T> const& fd)
{
  dbinfo_feature_data<T>& non_const = const_cast<dbinfo_feature_data<T>&>(fd);
  non_const.print(strm);
  return strm;
}
#if 0
template <class T>
vcl_ostream &operator<<(vcl_ostream &strm, dbinfo_feature_data<T> const* fd)
{
  if (fd)
    strm << *fd;
  else
    strm << "NULL Feature Data\n";
  return strm;
}
#endif

#define DBINFO_FEATURE_DATA_INSTANTIATE(T) \
template class dbinfo_feature_data<T >; \
template vcl_ostream& operator<<(vcl_ostream&, dbinfo_feature_data<T > const&); 
#if 0
template vcl_ostream& operator<<(vcl_ostream&, dbinfo_feature_data<T > const*);
#endif


