#ifndef dbrl_support_h_
#define dbrl_support_h_
#include <vcl_iostream.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>

// This class computes support of given pixel w.r.t to the point-tangents or edges in an image.
// The class has to be initialized with point-tangent image where angles take values 0-180 and non-edge takes 255
class dbrl_support:public vbl_ref_count{

public:
    dbrl_support(){radius_=5;};
    dbrl_support(vil_image_view<unsigned char> img){img_=img;radius_=5;};
    void set_neighborhood(int radius){radius_=radius;};
    double support(int i,int j,double angle);
    double compute_lambdap(int i,int j,int i0,int j0,double angle);
    double compute_lambda0(int i,int j,int i0,int j0);
    double cocircularity(double lambdap,double lamda0);
    double smoothness(double lambdap,double lamda0);
    double proximity(int i,int j, int i0,int j0);


     ~dbrl_support(){}
protected:

    vil_image_view<unsigned char> img_;
    int radius_;
};
    double compute_euler_spiral(double x0,double y0,double theta0,double x1,double y1,double theta1);

double compute_support(double x0,double y0,double theta0,double x1,double y1,double theta1,double stdrad);

#endif
