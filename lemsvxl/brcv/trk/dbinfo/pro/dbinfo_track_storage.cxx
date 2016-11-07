// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_track_storage.cxx

//:
// \file
#include <vcl_iostream.h>
#include <vsl/vsl_map_io.h>
#include <vsol/vsol_point_2d.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/dbinfo_observation.h>

//: Constructor
dbinfo_track_storage::dbinfo_track_storage()
{
}


//: Destructor
dbinfo_track_storage::~dbinfo_track_storage()
{
}


//: Return IO version number;
short 
dbinfo_track_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbinfo_track_storage::clone() const
{
  return new dbinfo_track_storage(*this);
}


//: Binary save self to stream.
void 
dbinfo_track_storage::b_write(vsl_b_ostream &os) const
{
  bool flag = true;
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  if(track_index_.size()){
    vsl_b_write(os, flag);
      vsl_b_write(os, track_index_);}
  else
    vsl_b_write(os, !flag);
  }


//: Binary load self from stream.
void 
dbinfo_track_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    bool flag = false;
    vsl_b_read(is, flag);
    if(flag)
      vsl_b_read(is, track_index_);
    break;
  }
  default:
    vcl_cerr << "I/O ERROR: dbinfo_track_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}



//: Return a smart pointer to the track given a track id
dbinfo_track_sptr
dbinfo_track_storage::track(const unsigned id)
{
  map_type::iterator mit;
  mit = track_index_.find(id);
  if(mit !=track_index_.end()) return mit->second; return 0;
  return 0;
}


//: Store the image in the storage class
void
dbinfo_track_storage::set_track(dbinfo_track_sptr const& track)
{
  if(!track)
    {
      vcl_cout << "In dbinfo_track_storage::set_track(..) - null track\n";
      return;
    }
  unsigned id = track->id();
  track_index_.insert(map_type::value_type(id, track));
}

void 
dbinfo_track_storage::set_tracks(vcl_vector<dbinfo_track_sptr> const& tracks)
{
  for(vcl_vector<dbinfo_track_sptr>::const_iterator trit = tracks.begin();
      trit != tracks.end(); ++trit)
    this->set_track(*trit);
}

vcl_vector<dbinfo_track_sptr> dbinfo_track_storage::tracks()
{
  vcl_vector<dbinfo_track_sptr> temp;
  for(map_type::iterator mit = track_index_.begin();
      mit != track_index_.end(); ++mit)
    temp.push_back((*mit).second);
  return temp;
}

//the current view of a track creates a vsol_polyline_2d from the 
//linked cogs of each track region. The track is recovered by matching
//the polyline vertex with the cog of the track observation
dbinfo_track_sptr 
dbinfo_track_storage::track_by_spacetime(const unsigned frame,
                                         vsol_point_2d_sptr const& v)
{
  assert(v);
  double dthresh = 0.1;
  vcl_vector<dbinfo_track_sptr> tracks = this->tracks();
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); trit++)
    {
      dbinfo_observation_sptr obs = (*trit)->observ(frame);
      vsol_point_2d_sptr cog = obs->geometry()->cog();
      double d = v->distance(cog);
      if(d<dthresh)
        return (*trit);
    }
  return (dbinfo_track*)0;
}

bool dbinfo_track_storage::delete_track(const unsigned track_id)
{
  map_type::iterator mit;
  mit = track_index_.find(track_id);
  if(mit ==track_index_.end()) 
    return false;
  track_index_.erase(mit);
  return true;
}
