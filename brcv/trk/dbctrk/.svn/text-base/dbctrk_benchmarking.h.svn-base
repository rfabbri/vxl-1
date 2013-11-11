#ifndef _dbctrk_benchmarking_h_
#define _dbctrk_benchmarking_h_
#include <vgl/vgl_polygon.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <vcl_string.h>
class dbctrk_benchmarking
{
public:
  static void in_or_out(vcl_vector<vgl_point_2d<double> > p, vcl_vector<dbctrk_tracker_curve_sptr> curves,
                        double &tp, double &tn);
  static bool write_moving_curves_to_ps(vcl_vector<dbctrk_tracker_curve_sptr> curves,
                                        vcl_string outputdir,int frameno);
  static bool read_outlines(vcl_string fileName,
                            vcl_vector<vgl_point_2d<double> > & v1);
  static bool matching_stats(vcl_string filename,
        vcl_vector<dbctrk_tracker_curve_sptr> curr_curves);


private:
  void dbctrk_bechmarking();

};

#endif
