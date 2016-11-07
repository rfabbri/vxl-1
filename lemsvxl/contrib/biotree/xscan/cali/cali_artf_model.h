#ifndef cali_artifact_model_h
#define cali_artifact_model_h

#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_quaternion.h>

class cali_artf_model
{
public:
    virtual void change_position(vnl_quaternion<double> const& artf_rot, 
                                                 vgl_point_3d<double> const& artf_trans) = 0;
    virtual double ray_artifact_intersect(vgl_homg_line_3d_2_points<double> const& line) = 0;
    virtual vgl_box_3d<double> bounding_box()=0;
    virtual double density()=0;
    virtual vcl_vector<vgl_point_3d<double> > ball_centers(void)=0;
    virtual vcl_vector<double> ball_radii(void)=0;
    virtual ~cali_artf_model(void) {};
};

#endif
