#include<dbctrk/dbctrk_tracker_curve.h>
#include<dbctrk/dbctrk_tracker_curve_sptr.h>
#include<mvl/FMatrix.h>
#include<vgl/vgl_point_3d.h>
#include<vgl/vgl_point_2d.h>
#include<utility>

class dbctrk_stereo_matching{

public:
  dbctrk_stereo_matching(){}
  dbctrk_stereo_matching(FMatrix F);
  ~dbctrk_stereo_matching(){}

  bool check_bounding_box_intersection(dbctrk_tracker_curve_sptr c1,
                                       dbctrk_tracker_curve_sptr c2);

  void stereo_matching();
  void set_curves1(std::vector<dbctrk_tracker_curve_sptr> c1);
  void set_curves2(std::vector<dbctrk_tracker_curve_sptr> c2);

  void get_curves1(std::vector<dbctrk_tracker_curve_sptr> & out1);
  void get_curves2(std::vector<dbctrk_tracker_curve_sptr> & out2);

  std::map<std::pair<int,int>, double >  spatiald1_;
  std::map<std::pair<int,int>, double >  spatiald2_;

  void bootstrap_matches(int level_curves);

  void compute_spatial_distance(std::vector<dbctrk_tracker_curve_sptr> curves,
                                std::map<std::pair<int,int>, double > & spatiald);
  double spatial_euclidean_dist(dbctrk_tracker_curve_sptr c1,
                                dbctrk_tracker_curve_sptr c2);
  double spatial_sig_dist(dbctrk_tracker_curve_sptr c1,
                          dbctrk_tracker_curve_sptr c2);



private:
  std::vector<dbctrk_tracker_curve_sptr> curves1_;
  std::vector<dbctrk_tracker_curve_sptr> curves2_;
  FMatrix F_;

  bool line_box_intersection(vgl_homg_line_2d<double> l,
                             vsol_box_2d_sptr box);
  bool line_segment_intersection(vgl_homg_line_2d<double> l,
                                 double xs,double xe, double ys, double ye);
};
