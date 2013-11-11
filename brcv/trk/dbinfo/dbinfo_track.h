// This is brl/bseg/dbinfo/dbinfo_track.h
#ifndef dbinfo_track_h_
#define dbinfo_track_h_
//---------------------------------------------------------------------
//:
// \file
// \brief the base class for tracks - currently just a set of observations
//
// \author
//  J.L. Mundy - April 12, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_track_geometry.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

class dbinfo_mi_track;



//The map from frame to observation
typedef vcl_map<unsigned, dbinfo_observation_sptr, vcl_less<unsigned> > obs_map;
//: The map from frame to transformation
typedef vcl_map<unsigned, vnl_matrix_fixed<float,3,3>, vcl_less<unsigned> > xform_map;

class dbinfo_track : public vbl_ref_count
{
 public:
  //: Constructors/destructor
  dbinfo_track(): id_(0), disabled_(false), start_frame_(0), end_frame_(),
    track_geom_(new dbinfo_track_geometry()), geom_valid_(false){}

  //:default destructor
  ~dbinfo_track(){}

  unsigned size() const {return observations_.size();}

  dbinfo_observation_sptr observ(unsigned frame) ;
  
  vgl_h_matrix_2d<float> xform(unsigned frame);

  void set_id(const unsigned id){id_ = id;}
  unsigned id()const {return id_;}

  //: The first frame of the track
  unsigned start_frame(){return start_frame_;}

  //: The last frame of the track
  unsigned end_frame(){return end_frame_;}

//: A numerical score measuring the compatibility of an observation with the track
virtual float score(dbinfo_observation_sptr const& obs) const {return 0;} 
   

//tracking related methods

//: Initialize the track with a specified observation
virtual void init_track(dbinfo_observation_sptr const& obs);

//: Extend the track 
virtual bool extend_track(dbinfo_observation_sptr const& obs);
//: Extend the track  when transformation is also provided.

virtual bool extend_track(dbinfo_observation_sptr const& obs, vgl_h_matrix_2d<float> h);

//: Disable track
virtual void disable()
{disabled_ = true;}

//: Disable track
virtual void enable()
{disabled_ = false;}

bool is_disabled() {return disabled_;}

//: The track geometry up to the current observation
dbinfo_track_geometry_sptr track_geometry();

//: Safe downcasting methods
virtual dbinfo_mi_track* cast_to_mi_track() { return 0; }
virtual dbinfo_mi_track const* cast_to_mi_track() const { return 0; }

//:Print information about self
virtual void print(vcl_ostream& os = vcl_cout) const;

//-----------------------
//:  BINARY I/O METHODS |
//-----------------------

//: Serial I/O format version
virtual unsigned version() const {return 2;}

//: Return a platform independent string identifying the class
virtual vcl_string is_a() const {return "dbinfo_track";}

//: determine if this is the given class
virtual bool is_class(vcl_string const& cls) const
{ return cls==is_a();}
  
//: Binary save self to stream.
virtual void b_write(vsl_b_ostream &os) const ;

//: Binary load self from stream.
virtual void b_read(vsl_b_istream &is);

protected:
//: recompute the track geometry for the entire track (after construction)
void reconstitute_track_geom();

//: the track id
unsigned id_;

//: disable flag
bool disabled_;

//: initial frame
unsigned start_frame_;

//: last frame
unsigned end_frame_;

//: the observations
obs_map observations_;

//: the transformations
xform_map xforms_;

//: the geometry of the track
dbinfo_track_geometry_sptr track_geom_;

//: a flag indicating valid geometry
bool geom_valid_;
};
//Inlined functions
inline void vsl_b_read(vsl_b_istream &is, dbinfo_track & trk)
{trk.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_track & trk)
{trk.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, dbinfo_track const* trk)
{
  //Don't allow writing for null feature data
  assert(trk);
  const dbinfo_track& ctrk = *trk;
  vsl_b_write(os, ctrk);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_track* &trk)
{
  delete trk;
  trk = new dbinfo_track();
  vsl_b_read(is, *trk);
}

inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_track const& trk)
{
  trk.print(os);
  return os;
}

inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_track const* trk)
{
  if (trk)
    os << *trk;
  else
    os << "NULL Feature \n";
  return os;
}

inline void vsl_print_summary(vcl_ostream& os, dbinfo_track const*  trk)
{os << trk;}



#include <dbinfo/dbinfo_track_sptr.h>

#endif // dbinfo_track_h_
