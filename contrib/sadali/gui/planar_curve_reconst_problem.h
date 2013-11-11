#ifndef planar_curve_reconst_problem__h
#define planar_curve_reconst_problem__h
#include <vnl/vnl_double_3x4.h>

#include <vnl/vnl_double_4x4.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>

class  planar_curve_reconst_problem : public vnl_least_squares_function

{
public:
    planar_curve_reconst_problem(const vnl_double_4x4 &BBMatrix, const vnl_double_3 &Proj_Point,
        const vnl_double_3x4 Proj)
        : vnl_least_squares_function( 3, 3, no_gradient)
    {
        ProjMat = Proj;
        Proj_Pt = Proj_Point;
        BB = BBMatrix;
        vnl_double_4 test_point(1.0,0.0,0.0,1.0);
        test_BB_point =BB*test_point;
       


    }
    ~planar_curve_reconst_problem()
    {
    }
    virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);
    void set_Proj(const vnl_double_3x4 &ProjMat);
    void set_Point(const vnl_double_3 &Point_image);
   




protected:
  

    vnl_double_3x4 ProjMat;
    vnl_double_3 Proj_Pt;
    vnl_double_4x4 BB;
    vnl_double_4 test_BB_point;
   

}
;
#endif
