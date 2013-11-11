// This is brl/bseg/dbinfo/dbinfo_feature_capture_process.h
#ifndef dbinfo_feature_capture_process_h_
#define dbinfo_feature_capture_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Captures feature data from a pre-stored track
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 26, 2004    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vpro/vpro_video_process.h>
#include <dbinfo/dbinfo_info_tracker.h>
#include <dbinfo/dbinfo_info_tracker_params.h>
class dbinfo_feature_capture_process : public vpro_video_process
{
 public:
  dbinfo_feature_capture_process(dbinfo_info_tracker_params& tp);
  ~dbinfo_feature_capture_process();
  virtual process_data_type get_output_type() { return TOPOLOGY; }

  //: track to next frame
  virtual bool execute();
  virtual bool finish();
  bool set_input_file(vcl_string const& file_name);
  bool set_output_file(vcl_string const& file_name);
 private:
  //members
  bool failure_;
  bool first_frame_;
  unsigned int face_index_;
  unsigned int start_frame_;
  vcl_string track_file_;
  vcl_string hist_file_;
  dbinfo_info_tracker tracker_;
  vcl_vector<vtol_face_2d_sptr> tracked_faces_;
  vcl_vector<vcl_vector<float> > tracked_hist_;
};


#endif // dbinfo_feature_capture_process_h_
