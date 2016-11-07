// This is brl/bseg/dbinfo/dbinfo_feature_data_base.h
#ifndef dbinfo_feature_data_base_h_
#define dbinfo_feature_data_base_h_
//---------------------------------------------------------------------
//:
// \file
// \brief the base class for tracking feature data
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
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_pixel_format.h>
#include <vcl_map.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_feature_format.h>
class dbinfo_feature_data_base : public vbl_ref_count
{
  typedef vcl_map<unsigned, unsigned, vcl_less<unsigned> > map_type;
 public:
  //: Constructors/destructor
  dbinfo_feature_data_base(): pix_format_(VIL_PIXEL_FORMAT_UNKNOWN),
    nplanes_(1), format_(DBINFO_FEATURE_FORMAT_UNKNOWN), 
    fixed_storage_(false), current_index_(0){}

  dbinfo_feature_data_base(const vil_pixel_format pix_format, 
                         const unsigned nplanes,
                         const dbinfo_feature_format format): 
    pix_format_(pix_format), nplanes_(nplanes),
    format_(format), fixed_storage_(false), current_index_(0){}

  ~dbinfo_feature_data_base(){}

  //: constructor for fixed size storage
  dbinfo_feature_data_base(const vil_pixel_format pix_format, 
                         const unsigned nplanes,
                         const dbinfo_feature_format format,
                         const unsigned size): pix_format_(pix_format),
    nplanes_(nplanes), format_(format), fixed_storage_(true),
    current_index_(0) 
    {frame_from_index_.resize(size); weights_.resize(size);}

  //: copy constructor
  dbinfo_feature_data_base(const dbinfo_feature_data_base& rhs)
    : pix_format_(rhs.pix_format_), nplanes_(rhs.nplanes_),
    format_(rhs.format_), frame_from_index_(rhs.frame_from_index_),
    index_from_frame_(rhs.index_from_frame_), weights_(rhs.weights_),
    fixed_storage_(rhs.fixed_storage_), current_index_(rhs.current_index_){}

  //:the image format used to form the data
  vil_pixel_format pix_format() const {return pix_format_;}

  //:the number of components of the image format
  unsigned nplanes() const {return nplanes_;}

  //:The feature type
  dbinfo_feature_format format() const {return format_;}

  //:Set a sample value to a specific frame
  inline void set_sample(const unsigned frame, const float w)
    {
      assert(fixed_storage_);
      assert(current_index_<size());
      frame_from_index_[current_index_]=frame;
      index_from_frame_.insert(map_type::value_type(frame,current_index_));
      weights_[current_index_]=w; 
      ++current_index_;
    }

  //:Add sample to incremental store
  inline void add_sample(const unsigned frame, const float w)
    {assert(!fixed_storage_);
    weights_.push_back(w); 
    frame_from_index_.push_back(frame); 
    ++current_index_;
    index_from_frame_.insert(map_type::value_type(frame,current_index_));}
  
  //:Get the video frame corresponding to the index
  inline unsigned frame(const unsigned index)
    {assert(index<size()); return frame_from_index_[index];}

  //:Get the storage index corresponding to the video frame
  inline unsigned index(const unsigned frame)
    {map_type::iterator mit;
    mit = index_from_frame_.find(frame);
    if(mit !=index_from_frame_.end()) return mit->second; return 0;}

  //:Get the first frame
  inline unsigned first_frame(){return frame_from_index_[0];}

  //:Get the current stored frame
  unsigned current_frame(){return frame_from_index_[current_index_];}

  //:Get the total number of frames in the database. Maintain consistency
  virtual unsigned size() const = 0;

  //:Get a weight value
  inline float w(const unsigned frame){return weights_[index(frame)];}

  //:Get storage mode, either fixed or cumulative over frames
  inline bool fixed() const {return fixed_storage_;}

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const = 0;

  //
  //: virtual binary I/O methods
  //

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const = 0;

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const = 0;
   
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const = 0;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is) = 0;

 protected:
  //:pixel format
  vil_pixel_format pix_format_;
  //:number of components
  unsigned nplanes_;
  dbinfo_feature_format format_;//!< Feature format that defines storage type
  vcl_vector<unsigned> frame_from_index_;//!< the frame corresponding to index
map_type index_from_frame_;//!< the index corresponding to frame
 vcl_vector<float> weights_;//!< weighting of frames for info computation
 bool fixed_storage_; //!< flag to specify if number of stored frames is fixed
unsigned current_index_;//!< current pointer into frame array
};
 
#include <dbinfo/dbinfo_feature_data_base_sptr.h>
#endif // dbinfo_feature_data_base_h_
