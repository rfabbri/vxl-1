#ifndef dbctrk_stereo_reconstruction_h_
#define dbctrk_stereo_reconstruction_h_

#include <mvl/FMatrix.h>
#include <mvl/PMatrix.h>
#include <utility>

class dbctrk_stereo_reconstruction
{
public:
  dbctrk_stereo_reconstruction();
  dbctrk_stereo_reconstruction(std::vector< std::vector<std::pair <vgl_point_2d<double>,vgl_point_2d<double> >  > > correspondences);


  void set_correspondences(std::vector< std::vector<std::pair<vgl_point_2d<double>,vgl_point_2d<double> >  > > correspondences);
  void set_F(FMatrix F);

  void compute_P();

  vgl_point_3d<double> triangulate_3d_point(const vgl_point_2d<double>& x1, const vgl_point_2d<double>& x2);

  void compute_3D_structure();
  ~dbctrk_stereo_reconstruction(){}

private:

  std::vector< std::vector<std::pair<vgl_point_2d<double>,vgl_point_2d<double> >  > > correspondences_;
  FMatrix F_;
  PMatrix P1_;
  PMatrix P2_;
  vnl_vector<double> e1_;
  vnl_vector<double> e2_;
        
};
#endif
