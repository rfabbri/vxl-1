#ifndef dbctrk_algs_h_
#define dbctrk_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Vishal Jain
// \brief curve tracking functions
//-----------------------------------------------------------------------------
//#include <btol/dll.h>
#include <map>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <vnl/vnl_double_2x2.h>
#include <dbctrk/dbctrk_curve.h>

class dbctrk_algs
{
 public:
  
  ~dbctrk_algs();

  static void
    smooth_curve(std::vector<vgl_point_2d<double> >& curve,double sigma=1.0);

  static vdgl_digital_curve_sptr
    create_digital_curves(std::vector<vgl_point_2d<double> > & curve);
  static vdgl_digital_curve_sptr
    create_digital_curves(Curve * c);
  static double compute_transformed_euclidean_distance
               (dbctrk_tracker_curve_sptr c1,
               dbctrk_tracker_curve_sptr c2,
               vnl_matrix<double> R,
               vnl_matrix<double> T,
               double s,std::map<int,int> alignment);

 static bool compute_transformation(std::vector<vgl_point_2d<double> > curve,
                              std::vector<vgl_point_2d<double> > & transformed_curve,
                              vnl_matrix<double> R,vnl_matrix<double> T,double scale);

 static  bool compute_transformation_next(std::vector<vgl_point_2d<double> > curve,
                   std::vector<vgl_point_2d<double> > & transformed_curve,
                    vnl_double_2x2 R,vnl_matrix<double> T,double scale);



 private:
  dbctrk_algs();
};

#endif
