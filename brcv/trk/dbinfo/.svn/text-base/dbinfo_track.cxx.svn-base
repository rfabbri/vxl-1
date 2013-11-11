#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>


void dbinfo_track::init_track(dbinfo_observation_sptr const& obs)
{
  //initialise storage elements
  observations_.clear();
  disabled_ = false; 
  track_geom_ = new dbinfo_track_geometry();
  start_frame_ = obs->frame();
  end_frame_ = obs->frame();
  //add the first observation to the track
  observations_.insert(obs_map::value_type(start_frame_, obs));
  vnl_matrix_fixed<float,3,3> h;
  h.set_identity();
  xforms_.insert(xform_map::value_type(start_frame_, h));
  //Update the track geometry
  track_geom_->set_id(this->id());
  track_geom_->set_start_frame(obs->frame());
  track_geom_->set_end_frame(obs->frame());
  dbinfo_region_geometry_sptr geom = obs->geometry();
  vsol_point_2d_sptr p = geom->cog();
  track_geom_->insert_point(obs->frame(),p);
}

bool dbinfo_track::extend_track(dbinfo_observation_sptr const& obs)
{
  if(disabled_)
    return false;
  observations_.insert(obs_map::value_type(obs->frame(), obs));
  //update the geometry
  end_frame_ = obs->frame();
  track_geom_->set_end_frame(end_frame_);
  dbinfo_region_geometry_sptr geom = obs->geometry();
  vsol_point_2d_sptr p = geom->cog();
  track_geom_->insert_point(end_frame_,p);
  vnl_matrix_fixed<float,3,3> h;
  h.set_identity();
  xforms_.insert(xform_map::value_type(obs->frame(), h));
  return true;
}

bool dbinfo_track::extend_track(dbinfo_observation_sptr const& obs, vgl_h_matrix_2d<float> h)
{
  if(disabled_)
    return false;
  observations_.insert(obs_map::value_type(obs->frame(), obs));
  xforms_.insert(xform_map::value_type(obs->frame(), h.get_matrix()));
  //update the geometry
  end_frame_ = obs->frame();
  track_geom_->set_end_frame(end_frame_);
  dbinfo_region_geometry_sptr geom = obs->geometry();
  vsol_point_2d_sptr p = geom->cog();
  track_geom_->insert_point(end_frame_,p);
  return true;
}
//check bounds on the track
dbinfo_observation_sptr dbinfo_track::observ(unsigned frame)
{
  if(start_frame_>frame||frame>end_frame_)
    return (dbinfo_observation*)0;
  return observations_[frame];
}

//check bounds on the track
vgl_h_matrix_2d<float> dbinfo_track::xform(unsigned frame)
{
  if(start_frame_>frame||frame>end_frame_)
  {
      vgl_h_matrix_2d<float> h;
      return h ;
  }
  return vgl_h_matrix_2d<float>(xforms_[frame]);
}

void dbinfo_track::reconstitute_track_geom()
{
  unsigned n = observations_.size();
  if(!n)
    return;
  track_geom_ = new dbinfo_track_geometry();
  track_geom_->set_id(this->id());

  track_geom_->set_start_frame(start_frame_);
  track_geom_->set_end_frame(start_frame_+ n-1);
  
  //insert the cog points
  for(unsigned i = 0; i<n; ++i)
    {
      dbinfo_observation_sptr obs = observations_[start_frame_+i];
      dbinfo_region_geometry_sptr geom = obs->geometry();
      vsol_point_2d_sptr p = geom->cog();
      bool isinserted =track_geom_->insert_point(obs->frame(),p);
      assert(isinserted);
    }
  geom_valid_ = true;
}

dbinfo_track_geometry_sptr dbinfo_track::track_geometry()
{
  if(!geom_valid_)
    this->reconstitute_track_geom();
  return track_geom_;
}

void dbinfo_track::print(vcl_ostream& os) const
{
  dbinfo_track& trk = (dbinfo_track&)(*this);  
  os << trk.is_a() << " [\n"
     << "id " << trk.id() << '\n'
     << "n observations " << trk.size() << '\n';
  if(trk.size())
    os << "observation[0] \n"
       << *(trk.observ(0));
  os << "]\n";
}


// The serialization can be done here in the base class, since for now   
// the sub-classes have no independent data of their own, just 
// more specialized methods

//: Binary save self to stream.
void dbinfo_track::b_write(vsl_b_ostream &os) const
{
  dbinfo_track& trk = (dbinfo_track&)(*this);  
  //Must have data
  assert(trk.size()>0);
  vsl_b_write(os, trk.version());
  vsl_b_write(os, trk.id());
  vsl_b_write(os, start_frame_);
  vsl_b_write(os, end_frame_);
  vsl_b_write(os, observations_);
  vsl_b_write(os, xforms_);
}

//: Binary load self from stream.
void dbinfo_track::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, id_);
        vcl_vector<dbinfo_observation_sptr> obs;
        vsl_b_read(is, obs);
        unsigned n = obs.size();
        for(unsigned i = 0; i<n; ++i)
          {
            dbinfo_observation_sptr ob = obs[i];
            observations_.insert(obs_map::value_type(ob->frame(),ob));
          }
        start_frame_ = obs[0]->frame();
        end_frame_ = obs[n-1]->frame();
        break;
      }
    case 2:
      {
        vsl_b_read(is, id_);
        vsl_b_read(is, start_frame_);
        vsl_b_read(is, end_frame_);
        vsl_b_read(is, observations_);
        vsl_b_read(is, xforms_);
        break;
      }
    default:
      vcl_cout << "In dbinfo_track::b_read(..) - unknown binary I/O version\n";
    }
}
