#include<dbctrk/dbctrk_stereo_reconstruction.h>
#include<vnl/vnl_double_4x4.h>
#include<vnl/vnl_double_3x4.h>
#include<vnl/algo/vnl_svd.h>
#include<vnl/vnl_double_4.h>
#include <vcl_iostream.h>
#include<vgl/algo/vgl_homg_operators_2d.h>
#include <brct/brct_algos.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
dbctrk_stereo_reconstruction::dbctrk_stereo_reconstruction()
{
correspondences_.clear();
}
dbctrk_stereo_reconstruction::dbctrk_stereo_reconstruction(vcl_vector<vcl_vector<vcl_pair <vgl_point_2d<double>,vgl_point_2d<double> >  > > correspondences)
{
correspondences_=correspondences;
}
void dbctrk_stereo_reconstruction::compute_P()
{
  vnl_double_3x4 P1;

  P1[0][0] = 1;     P1[0][1] = 0;      P1[0][2] = 0;        P1[0][3] = 0;
  P1[1][0] = 0;     P1[1][1] = 1;      P1[1][2] = 0;        P1[1][3] = 0;
  P1[2][0] = 0;     P1[2][1] = 0;      P1[2][2] = 1;        P1[2][3] = 0;

 vnl_double_3x3 K;
 K[0][0]=2000;K[0][1]=0;K[0][2]=0;
 K[1][0]=0;K[1][1]=2000;K[1][2]=0;
 K[2][0]=0;K[2][1]=0;K[2][2]=1;


  //P1_=K*P1;
  
  P1_.set(P1);
  vnl_double_3x3 ex;
  ex[0][0] = 0;     ex[0][1] = -e2_[2];  ex[0][2] = e2_[1];
  ex[1][0] = e2_[2];  ex[1][1] = 0;      ex[1][2] = -e2_[0];
  ex[2][0] = -e2_[1]; ex[2][1] = e2_[0];   ex[2][2] = 0;

  //ex*=F_;
  vnl_double_3x3 A=ex*F_.get_matrix();
  P2_.set(A,e2_);
  //P2_.set(K*P2_.get_matrix());
  
}
vgl_point_3d<double> dbctrk_stereo_reconstruction::triangulate_3d_point(const vgl_point_2d<double>& x1, const vgl_point_2d<double>& x2)
{
  vnl_double_4x4 A;
  vnl_double_3x4 P1,P2;

  P1=P1_.get_matrix();
  P2=P2_.get_matrix();
  
  for (int i=0; i<4; i++){
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }

  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}
  
void dbctrk_stereo_reconstruction::compute_3D_structure()
{
  vcl_ofstream out("c:\\pts3d.vrml");
  
  vcl_vector<vsol_point_3d_sptr> pts3d;
  vcl_vector<vcl_vector<vsol_point_3d_sptr> > curves3d;
   
  vcl_ofstream ofile("c:\\3dpoints.txt");
  for(unsigned int i=0;i<correspondences_.size();i++)
  {
    pts3d.clear();
    for(unsigned int j=0;j<correspondences_[i].size();j++)
    {
      vgl_point_3d<double> p3D=triangulate_3d_point(correspondences_[i][j].first,correspondences_[i][j].second);
      vcl_cout<<p3D.x()<<" "<<p3D.y()<<" "<<p3D.z()<<"\n";
      vsol_point_3d_sptr p=new vsol_point_3d(p3D);
      pts3d.push_back(p);
    }
    //ofile<<p3D.x()<<" "<<p3D.y()<<" "<<p3D.z()<<"\n";
    curves3d.push_back(pts3d);
  }
  brct_algos::write_vrml_header(out);
  //brct_algos::write_vrml_curves(out, curves3d);
  brct_algos::write_vrml_trailer(out);
  out.close();
}


void dbctrk_stereo_reconstruction::set_F(FMatrix F)
{
  F_=F;
  vgl_homg_point_2d<double> e1;
  vgl_homg_point_2d<double> e2;

  F_.get_epipoles(e1,e2);

  e1_=vgl_homg_operators_2d<double>::get_vector(e1);
  e2_=vgl_homg_operators_2d<double>::get_vector(e2);
  
}
