// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_track_storage.h
#ifndef dbinfo_track_storage_h_
#define dbinfo_track_storage_h_

//:
// \file
// \brief The bpro1 storage class for a set of dbinfo_track(s)
// \author J.L. Mundy
// \date June 1, 2005
//
//
// \verbatim
//  Modifications
// \endverbatim
#include <vcl_map.h>
#include <bpro1/bpro1_storage.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>

class dbinfo_track_storage : public bpro1_storage {

  typedef vcl_map<unsigned, dbinfo_track_sptr, vcl_less<unsigned> > map_type;

public:

  dbinfo_track_storage();
  virtual ~dbinfo_track_storage();
  virtual vcl_string type() const { return "dbinfo_track_storage"; }
  

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbinfo_track_storage"; }

  //:set a single track
  void set_track(dbinfo_track_sptr const& track);

  //:set multiple tracks
  void set_tracks(vcl_vector<dbinfo_track_sptr> const& tracks);
  
  //:retrieve a track by id
  dbinfo_track_sptr track(const unsigned id);

  //:retrieve all tracks
  vcl_vector<dbinfo_track_sptr> tracks();

  //:retrive a track by spacetime position
  dbinfo_track_sptr track_by_spacetime(const unsigned frame,
                                       vsol_point_2d_sptr const& cog);

  bool delete_track(const unsigned track_id);

 protected:

  map_type track_index_;
  
};

//Inlined functions
inline void vsl_b_read(vsl_b_istream &is, dbinfo_track_storage & trk_st)
{trk_st.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_track_storage & trk_st)
{trk_st.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_track_storage* trk_st)
{
  //Don't allow writing for null feature data
  assert(trk_st);
  trk_st->b_write(os);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_track_storage* &trk_st)
{
  delete trk_st;
  trk_st = new dbinfo_track_storage();
  trk_st->b_read(is);
}

//: Create a smart-pointer to a dbinfo_track_storage.
struct dbinfo_track_storage_new : public dbinfo_track_storage_sptr
{
  typedef dbinfo_track_storage_sptr base;

  //: Constructor - creates a default dbinfo_track_storage_sptr.
  dbinfo_track_storage_new() : base(new dbinfo_track_storage()) { }
};

#endif // dbinfo_track_storage_h_
