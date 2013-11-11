#ifndef bprt_homog_interface_h_
#define bprt_homog_interface_h_

#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>

#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <mvl/HMatrix2D.h>
#include <HMatrix2DComputeMLESAC.h>


class params
{ private:
        double normx,normy;
double dist;

public:
        params(){
        };
        params(double normx,double normy,double dist){
                this->normx=normx;
                this->normy=normy;
                this->dist=dist;
        }
        ~params()
        {
        };
inline  set_params(double normx,double normy,double dist){
                this->normx=normx;
                this->normy=normy;
                this->dist=dist;
        }
inline get_params(double &a,double &b, double &d)
{
        (a)=normx;
        (b)=normy;
        (d)=dist;
}
        

}
;
class bprt_plane_reconst
{
private:


vnl_double_3x3  K1_;
vnl_double_3x3  K2_;
vnl_double_3x3  R_;
vnl_double_3 t_;

HMatrix2D H_;
double std_;
vcl_vector <vgl_homg_point_2d <double> > ptlist1_,ptlist2_;

public: 
        params param_;
bprt_plane_reconst();
bprt_plane_reconst(vcl_vector <vtol_edge_2d_sptr >  curvs1,vcl_vector <vtol_edge_2d_sptr >  curvs2,int samp,double std);
~bprt_plane_reconst();
bool findplaneparams(HMatrix2D H,vnl_double_3x3 K1,vnl_double_3x3 K2,
                                        vnl_double_3x3 R,vnl_double_3 t);
vdgl_digital_curve_sptr conv_vtol_to_dc(vtol_edge_2d_sptr vtoledge);
void comp_param();
void set_cal_params(vnl_double_3x3 K1,vnl_double_3x3 K2,vnl_double_3x3 R,vnl_double_3 t);
vcl_vector< vsol_spatial_object_2d_sptr> get_vsol_point_2ds(int view);
int calibrate();
}
;



#endif


