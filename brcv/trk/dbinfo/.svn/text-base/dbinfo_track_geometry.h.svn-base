// This is brl/bseg/dbinfo/dbinfo_track_geometry.h
#ifndef dbinfo_track_geometry_h_
#define dbinfo_track_geometry_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The geometry of a track
//
// \author
//  J.L. Mundy - September 25, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//---------------------------------------------------------------------
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

typedef vcl_map<unsigned, vsol_point_2d_sptr, vcl_less<unsigned> > track_map;

class dbinfo_track_geometry : public vbl_ref_count
{
  //: Constructors/destructor
 public:
  dbinfo_track_geometry(); // default
  
  ~dbinfo_track_geometry();
  //: the id corresponds to the track from which the geometry is derived
  void set_id(const unsigned id){id_ = id;}
  unsigned id()const {return id_;}
  //: the start frame and end frame of the track
  void set_start_frame(unsigned frame){start_frame_=frame;}
  void set_end_frame(unsigned frame){end_frame_=frame;}
  unsigned start_frame(){return start_frame_;}
  unsigned end_frame(){return end_frame_;}

  //:insert the cog of the tracking polygon as the track position
  bool insert_point(unsigned frame, vsol_point_2d_sptr const& point);

  //:retrieve the point at the specified frame
  vsol_point_2d_sptr point(unsigned frame);

  //:get a snapshot of the track between frames
  vsol_polyline_2d_sptr incremental_track(const unsigned initial_frame,
                                          const unsigned end_frame);
  //:get a snapshot of the track from the start to the current frame
  vsol_polyline_2d_sptr current_track();

 protected:
  unsigned start_frame_;
  unsigned end_frame_;
  unsigned id_;
  track_map point_index_;
};
#include <dbinfo/dbinfo_track_geometry_sptr.h>

#endif // dbinfo_track_geometry_h_
