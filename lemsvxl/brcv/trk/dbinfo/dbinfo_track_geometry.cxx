#include <vcl_vector.h>
#include <dbinfo/dbinfo_track_geometry.h>
#include <vsol/vsol_polyline_2d.h>

//constructors/destructor
dbinfo_track_geometry::dbinfo_track_geometry() 
{
}

dbinfo_track_geometry::~dbinfo_track_geometry() 
{
}

//:scan gradient data from the current frame into the feature data store
bool dbinfo_track_geometry::
insert_point(unsigned frame, vsol_point_2d_sptr const& point)
{
  if(frame<start_frame_||frame>end_frame_)
    return false;
  assert(point.ptr());
  point_index_.insert(track_map::value_type(frame, point));
  return true;
}

vsol_point_2d_sptr dbinfo_track_geometry::point(unsigned frame)
{
  vsol_point_2d_sptr p;
  if(frame<start_frame_||frame>end_frame_)
    return p;
  p = point_index_[frame];
  return p;
}

//get a snapshot of the track between frames
vsol_polyline_2d_sptr 
dbinfo_track_geometry::incremental_track(const unsigned initial_frame,
                                         const unsigned end_frame)
{
  vcl_vector<vsol_point_2d_sptr> points;
  unsigned endf = end_frame;
  if(end_frame>end_frame_)
    endf = end_frame_;
  
  points.push_back(this->point(initial_frame));
  for(unsigned i = initial_frame+1; i<=endf; ++i)
    {
      vsol_point_2d_sptr p = this->point(i);
      assert(p);
      points.push_back(p);
    }
  if(points.size()==1)//zero length curve
    points.push_back(this->point(initial_frame));
  return new vsol_polyline_2d(points);
}

//get a snapshot of the track from the start to the current frame
vsol_polyline_2d_sptr dbinfo_track_geometry::current_track()
{
  return this->incremental_track(start_frame_, end_frame_);
}
