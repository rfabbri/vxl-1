//This is lemsvxl/brcv/shp/dbskr/tests/test_hypergraph.cxx
//  Ming-Ching Chang  Mar 11, 2005
//
//
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//    or 
//  dbsk3dr_test_rigid_trans D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//

#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix_fixed.h>

#include <dbgl/dbgl_rigid_align.h>

#include <dbmsh3dr/dbmsh3dr_match.h>

//Match P3 to P2, the result is in PP.
void EuclideanTransform3D_test ()
{
  vcl_vector<vgl_point_3d<double> > PS0, PS1, PS2;

  PS0.push_back (vgl_point_3d<double> (3, 3, 0));
  PS0.push_back (vgl_point_3d<double> (2, 3, 0));
  PS0.push_back (vgl_point_3d<double> (1, 2, 0));
  PS0.push_back (vgl_point_3d<double> (1, 1, 0));
  
  PS1.push_back (vgl_point_3d<double> (3, 3, 0));
  PS1.push_back (vgl_point_3d<double> (2, 3, 0));
  PS1.push_back (vgl_point_3d<double> (1, 2, 0));
  PS1.push_back (vgl_point_3d<double> (1, 1, 0));

  PS2.push_back (vgl_point_3d<double> (101, 101, 200));
  PS2.push_back (vgl_point_3d<double> (102, 101, 200));
  PS2.push_back (vgl_point_3d<double> (103, 102, 200));
  PS2.push_back (vgl_point_3d<double> (103, 103, 200));

  vnl_matrix_fixed<double,3,3> R (0.0);
  vnl_vector_fixed<double,3> C1 (0.0);
  vnl_vector_fixed<double,3> C2 (0.0);

  dbgl_get_rigid_alignment (PS1, PS2, R, C1, C2);

  vcl_vector<double> ppx, ppy, ppz;
  for (unsigned int i=0; i<PS1.size(); i++) {
    double p[3], pp[3];
    p[0]= PS2[i].x();
    p[1]= PS2[i].y();
    p[2]= PS2[i].z();
    vnl_matrix<double> Point (p, 3, 1);
    vnl_matrix<double> PointP;
    PointP=R*Point;

    pp[0] = PointP.get(0,0);
    pp[1] = PointP.get(1,0);
    pp[2] = PointP.get(2,0);

    pp[0] += C1.get(0);
    pp[1] += C1.get(1);
    pp[2] += C1.get(2);

    TEST("x_i ", vcl_fabs(pp[0] - PS0[i].x()) < 1E-5, true);
    TEST("y_i ", vcl_fabs(pp[1] - PS0[i].y()) < 1E-5, true);
    TEST("z_i ", vcl_fabs(pp[2] - PS0[i].z()) < 1E-5, true);

    ppx.push_back (pp[0]);
    ppy.push_back (pp[1]);
    ppz.push_back (pp[2]);
  }
}


void test_2d_euclidean_transform ()
{
  const int N = 4;
  vcl_vector<double> x1,y1,x2,y2;

  x1.push_back (3);
  x1.push_back (2);
  x1.push_back (1);
  x1.push_back (1);

  y1.push_back (3);
  y1.push_back (3);
  y1.push_back (2);
  y1.push_back (1);

  x2.push_back (101);
  x2.push_back (102);
  x2.push_back (103);
  x2.push_back (103);

  y2.push_back (101);
  y2.push_back (101);
  y2.push_back (102);
  y2.push_back (103);


  //1)Compute Centroid
  double x1_centroid=0, x2_centroid=0, y1_centroid=0, y2_centroid=0;
  for (int j=0; j<N; j++) {
    x1_centroid+=x1[j];
    x2_centroid+=x2[j];
    y1_centroid+=y1[j];
    y2_centroid+=y2[j];
  }
  x1_centroid = x1_centroid/N;
  x2_centroid = x2_centroid/N;
  y1_centroid = y1_centroid/N;
  y2_centroid = y2_centroid/N;

  //2)Centering the data
  for (int j=0; j<N; j++) {
    x1[j] -= x1_centroid;
    x2[j] -= x2_centroid;

    y1[j] -= y1_centroid;
    y2[j] -= y2_centroid;
  } 
  //3)Computing covariance matrix
  double H[4] = {0,0,0,0};
   for (int j=0; j<N; j++) {
    H[0] += x2[j]*x1[j];
    H[1] += x2[j]*y1[j];
    H[2] += y2[j]*x1[j];
    H[3] += y2[j]*y1[j];
  }

  //4)Computing svd
  vnl_matrix<double> M (H, 2, 2);
  vnl_svd<double> svd (M, 1e-10);
  vnl_matrix<double> U;
  vnl_matrix<double> Ut;
  vnl_matrix<double> V;

  U=svd.U();
  V=svd.V();
  Ut=U.transpose();
  vnl_matrix<double> R = V*Ut;
  //R is the rotation matrix

  //5)Compute the translation.
  //  Translation is just the difference between two centers
  //double tx=0, ty=0; // double theta=vcl_acos(R(0,0));
  double center1[2] = {x1_centroid,y1_centroid};
  double center2[2] = {x2_centroid,y2_centroid};

  vnl_matrix<double> cen1 (center1,2,1);
  vnl_matrix<double> cen2 (center2,2,1);

  //6)Rotate and translate the (x2, y2) to be (x1p, y1p)
  vcl_vector<double> x1p, y1p;
  for (int i=0; i<N; i++) {
    double p[2], pr[2];
    p[0]= x2[i];
    p[1]= y2[i];
    vnl_matrix<double> Point (p, 2, 1);
    vnl_matrix<double> PointR;
    PointR=R*Point;

    pr[0] = PointR.get(0,0);
    pr[1] = PointR.get(1,0);

    pr[0] += x1_centroid;
    pr[1] += y1_centroid;

    x1p.push_back (pr[0]);
    y1p.push_back (pr[1]);
  }
}



MAIN_ARGS(dbsk3dr_test_rigid_trans)
{
  vcl_string dir_base;
  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }

  testlib_test_start("testing rigid transformation.");

  EuclideanTransform3D_test ();

  test_2d_euclidean_transform ();

  return testlib_test_summary();
}

