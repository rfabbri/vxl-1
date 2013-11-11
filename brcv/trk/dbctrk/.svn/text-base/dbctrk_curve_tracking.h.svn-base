#ifndef dbctrk_curve_tracking_h_
#define dbctrk_curve_tracking_h_

#include <vtol/vtol_edge_2d_sptr.h>
#include <vcl_vector.h>

#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_curve_matching.h>
#include <dbctrk/dbctrk_curve_clustering.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>
#include <dbspi/dbspi_curve_map.h>
#include<vcl_iostream.h>
class dbctrk_curve_tracking_params
{
 public:
  // Parameters

  bool clustering_;
  bool transitive_closure;
  bool benchmark_;
  int window_size;
  int min_length_of_curves;
  bool ps_moving_curves;
  bool ps_match_curves;
  bool isIHS;
  
  int thetabins;
  float r1;
  float v1;
  
  vcl_map<int,vcl_vector< vgl_point_2d<double> > > contours_;

  dbctrk_curve_matching_params mp ;
  dbctrk_curve_clustering_params cp ;
  vcl_string ps_file_directory;

  dbctrk_curve_tracking_params(){window_size=2;isIHS=true;}

  dbctrk_curve_tracking_params(dbctrk_curve_matching_params &mpa,
                             dbctrk_curve_clustering_params &cpa,
                             bool clustering,int min_len)
  { mp=mpa;cp=cpa,clustering_=clustering;min_length_of_curves=min_len;}

  ~dbctrk_curve_tracking_params(){}
  void operator=(dbctrk_curve_tracking_params tp);
  void print_summary(vcl_ostream &os) const;
};
vcl_ostream& operator<<(vcl_ostream& s, dbctrk_curve_tracking_params const& params);

class dbctrk_curve_tracking
{
 public:
  // Input : for each image, for each
  dbctrk_curve_tracking(){}
  dbctrk_curve_tracking(dbctrk_curve_tracking_params &tp);
  ~dbctrk_curve_tracking(){}
  // list of input curves for curve-tracking
  vcl_vector<vcl_vector< vtol_edge_2d_sptr > > input_curves_;

  // Output
  vcl_vector<vcl_vector<dbctrk_tracker_curve_sptr > > output_curves_;
  // Output to compute transitive closure
  vcl_vector<vcl_vector<dbctrk_tracker_curve_sptr > > output_curves_tc_;

  vcl_vector<vil_image_view<float> > plane0;
  vcl_vector<vil_image_view<float> > plane1;
  vcl_vector<vil_image_view<float> > plane2;

  vcl_vector<vil_image_view<float> > dtimgs;
  vcl_vector<vil_image_view<int> > dtmaps;


#if 0
  // Params
  dbctrk_curve_tracker_params params_;
  dbctrk_curve_tracking(dbctrk_curve_tracker_params p){ params_ = p; }
#endif // 0

  vcl_vector<vcl_vector<vtol_edge_2d_sptr > > * get_input(){ return & input_curves_; }
  void set_input(vcl_vector< vcl_vector< vtol_edge_2d_sptr > > curve){ input_curves_=curve; }

  vcl_vector< vcl_vector< dbctrk_tracker_curve_sptr> > * get_output(){ return & output_curves_; }
  vcl_vector< vcl_vector< dbctrk_tracker_curve_sptr> > * get_output_tc(){ return & output_curves_tc_; }
  void set_output(vcl_vector< vcl_vector< dbctrk_tracker_curve_sptr > > curve){ output_curves_=curve; }

  double test_output(vcl_vector<vgl_point_2d<double> > points,int t);
  // returns no of curves in the given frame
  int get_output_size_at(unsigned int frame)
  {
    if (frame>=output_curves_.size())
      return -1;
    else
      return output_curves_[frame].size();
  }

  vcl_vector< dbctrk_tracker_curve_sptr> *get_output_curves(unsigned int frame_no);
  dbctrk_tracker_curve_sptr get_output_curve(unsigned int frame_no, int set_id);

  void write_clusters(vcl_string filename,int i);
  void get_reliable_curves(unsigned int frame_no, unsigned int window_sz);
  // tracking of  the sequence
  void track();
  // tracking for the given frame
  void track_frame(unsigned int frame);
  void write_results(vcl_string name);
  void write_tracks(dbctrk_tracker_curve_sptr curve,vcl_string fname,int min_length_of_track);
  // traversal function to get tracks
  void level_order_traversal(dbctrk_tracker_curve_sptr curve,vcl_list<dbctrk_tracker_curve_sptr> & tr);
  void obtain_tracks();
  int get_min_len_of_curves(){return tp_.min_length_of_curves;}
  vcl_map<int,int> len_of_tracks_;
  vcl_string outfilename;
  vcl_vector<vcl_vector<dbctrk_tracker_curve_sptr> > moving_curves_;
  vcl_vector<float> scores_;
  vcl_vector<vil1_memory_image_of<vil1_rgb<unsigned char> > > images_;
  void set_tracking_params(dbctrk_curve_tracking_params &tp);
  short version();
  void b_write(vsl_b_ostream & os);
  void b_read(vsl_b_istream & is);
  dbspi_curve_map * chamfermap_;
  bool compute_chamfer_image(int frame);
 protected:
   
   dbctrk_curve_tracking_params tp_; 
   vcl_vector<vcl_vector<vcl_vector<dbctrk_tracker_curve_sptr> > > frame_moving_curves_;

 private:
 // current frame number 
  int frame_;
  bool clustering_;
 
  int min_len_of_curves_;
};




//: Binary save dbctrk_tracker_curve_sptr to stream.
//void vsl_b_write(vsl_b_ostream & os, const vcl_vector<dbctrk_tracker_curve_sptr> &p);
//: Binary load dbctrk_tracker_curve_sptr to stream
//void vsl_b_read(vsl_b_istream &is, vcl_vector<dbctrk_tracker_curve_sptr> &p);
#endif
