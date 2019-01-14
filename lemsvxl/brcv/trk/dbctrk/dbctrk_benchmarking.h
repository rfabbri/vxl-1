#ifndef _dbctrk_benchmarking_h_
#define _dbctrk_benchmarking_h_
#include <vgl/vgl_polygon.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <string>
class dbctrk_benchmarking
{
public:
  static void in_or_out(std::vector<vgl_point_2d<double> > p, std::vector<dbctrk_tracker_curve_sptr> curves,
                        double &tp, double &tn);
  static bool write_moving_curves_to_ps(std::vector<dbctrk_tracker_curve_sptr> curves,
                                        std::string outputdir,int frameno);
  static bool read_outlines(std::string fileName,
                            std::vector<vgl_point_2d<double> > & v1);
  static bool matching_stats(std::string filename,
        std::vector<dbctrk_tracker_curve_sptr> curr_curves);


private:
  void dbctrk_bechmarking();

};

#endif
