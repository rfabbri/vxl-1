//: 
// \file     test_edge_det_nonmaximum_suppression
// \brief    testing for nonmaxium_suppression for edge detector
// \author   Can Aras / can@lems.brown.edu
// \date     2006-01-25
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_cylinder_map.h>
#include <det/edge_det_nonmaximum_suppression.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_erf.h>

#include "test_det_edge_line_2d.h"
#include "test_det_edge_line_3d.h"
#include "test_det_edge_circle_2d.h"
#include "test_det_edge_circle_3d.h"

static void test_edge_det_nonmaximum_suppression()
{ 
  edge_det_nonmaximum_suppression suppress;
  TEST("look-up table-1 test-0", suppress.vertex_offset_indices(0), vnl_int_3(-1,-1,-1));
  TEST("look-up table-1 test-1", suppress.vertex_offset_indices(1), vnl_int_3(0,-1,-1));
  TEST("look-up table-1 test-2", suppress.vertex_offset_indices(2), vnl_int_3(1,-1,-1));
  TEST("look-up table-1 test-3", suppress.vertex_offset_indices(3), vnl_int_3(-1,0,-1));
  TEST("look-up table-1 test-4", suppress.vertex_offset_indices(4), vnl_int_3(0,0,-1));
  TEST("look-up table-1 test-5", suppress.vertex_offset_indices(5), vnl_int_3(1,0,-1));
  TEST("look-up table-1 test-6", suppress.vertex_offset_indices(6), vnl_int_3(-1,1,-1));
  TEST("look-up table-1 test-7", suppress.vertex_offset_indices(7), vnl_int_3(0,1,-1));
  TEST("look-up table-1 test-8", suppress.vertex_offset_indices(8), vnl_int_3(1,1,-1));
  TEST("look-up table-1 test-9", suppress.vertex_offset_indices(9), vnl_int_3(-1,-1,0));
  TEST("look-up table-1 test-10", suppress.vertex_offset_indices(10), vnl_int_3(0,-1,0));
  TEST("look-up table-1 test-11", suppress.vertex_offset_indices(11), vnl_int_3(1,-1,0));
  TEST("look-up table-1 test-12", suppress.vertex_offset_indices(12), vnl_int_3(-1,0,0));
  TEST("look-up table-1 test-13", suppress.vertex_offset_indices(13), vnl_int_3(1,0,0));
  TEST("look-up table-1 test-14", suppress.vertex_offset_indices(14), vnl_int_3(-1,1,0));
  TEST("look-up table-1 test-15", suppress.vertex_offset_indices(15), vnl_int_3(0,1,0));
  TEST("look-up table-1 test-16", suppress.vertex_offset_indices(16), vnl_int_3(1,1,0));
  TEST("look-up table-1 test-17", suppress.vertex_offset_indices(17), vnl_int_3(-1,-1,1));
  TEST("look-up table-1 test-18", suppress.vertex_offset_indices(18), vnl_int_3(0,-1,1));
  TEST("look-up table-1 test-19", suppress.vertex_offset_indices(19), vnl_int_3(1,-1,1));
  TEST("look-up table-1 test-20", suppress.vertex_offset_indices(20), vnl_int_3(-1,0,1));
  TEST("look-up table-1 test-21", suppress.vertex_offset_indices(21), vnl_int_3(0,0,1));
  TEST("look-up table-1 test-22", suppress.vertex_offset_indices(22), vnl_int_3(1,0,1));
  TEST("look-up table-1 test-23", suppress.vertex_offset_indices(23), vnl_int_3(-1,1,1));
  TEST("look-up table-1 test-24", suppress.vertex_offset_indices(24), vnl_int_3(0,1,1));
  TEST("look-up table-1 test-25", suppress.vertex_offset_indices(25), vnl_int_3(1,1,1));

  /*  int dim = 26;
  int kernel = 7;
  int margin = (kernel-1)/2;
  det_cylinder_map cm(dim, dim, dim);
  for(int angle=10; angle<=45; angle=angle+5)
  {
  for(int intc=-10; intc<=10; intc++)
  {
  create_2d_line_data_analytically_v2(angle, intc, cm);
  det_cylinder_map res_cm(suppress.apply(cm));
  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\subpixels_";
  char buffer[10];
  fname = fname + "angle=" + itoa(angle, buffer, 10) + ",c=" + itoa(intc, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  for(int j=margin+1; j<dim-(margin+1); j++)
  {
  for(int i=margin+1; i<dim-(margin+1); i++)
  {
  if(res_cm[i][j][1].strength_ > 10e-06)
  {
  vcl_cout << i << ' ' << j << ' ';
  vcl_cout << res_cm[i][j][1].strength_ << ' ';
  vcl_cout << res_cm[i][j][1].location_ << '\n';
  vcl_fprintf(fp, "%f %f\n", i+res_cm[i][j][1].location_.x(), j+res_cm[i][j][1].location_.y());
  }
  }
  vcl_cout << '\n';
  }
  vcl_fclose(fp);
  }
  }*/
  /*
  int dim = 26;
  int kernel = 7;
  int margin = (kernel-1)/2;
  det_cylinder_map cm(dim, dim, dim);
  for(int radius=3; radius<=10; radius++)
  {
  //    for(int centre_x=5; centre_x<=10; centre_x++)
  //    {
  //      for(int centre_y=5; centre_y<=10; centre_y++)
  //      {
  int centre_x = 10;
  int centre_y = 10;
  create_2d_circle_data_analytically_v2(radius, centre_x, centre_y, cm);
  det_cylinder_map res_cm(suppress.apply(cm));
  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\subpixels_";
  char buffer[10];
  fname = fname + "radius=" + itoa(radius, buffer, 10) + ",center=" + itoa(centre_x, buffer, 10) + 
  "," + itoa(centre_y, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  for(int j=margin+1; j<dim-(margin+1); j++)
  {
  for(int i=margin+1; i<dim-(margin+1); i++)
  {
  if(res_cm[i][j][1].strength_ > 10e-6)
  {
  vcl_cout << i << ' ' << j << ' ';
  vcl_cout << res_cm[i][j][1].strength_ << ' ';
  vcl_cout << res_cm[i][j][1].location_ << '\n';
  vcl_fprintf(fp, "%f %f\n", i+res_cm[i][j][1].location_.x(), j+res_cm[i][j][1].location_.y());
  }
  }
  vcl_cout << '\n';
  }
  vcl_fclose(fp);
  //     }
  //   }
  }*/

  /*
  int dim = 26;
  int kernel = 7;
  int margin = (kernel-1)/2;
  det_cylinder_map cm(dim, dim, dim);
  for(int theta=0; theta<=90; theta=theta+5)
  {
  for(int phi=0; phi<=90; phi=phi+5)
  {
  for(int intd=-100; intd<=-90; intd++)
  {
  create_3d_line_data_analytically_v2(theta, phi, intd, cm);
  det_cylinder_map res_cm(suppress.apply(cm));

  double maximum = 0;
  for(int k=(margin+1); k<dim-(margin+1); k++)
  {
  for(int j=(margin+1); j<dim-(margin+1); j++)
  {
  for(int i=(margin+1); i<dim-(margin+1); i++)
  {
  if(res_cm[i][j][k].strength_ > maximum)
  maximum = res_cm[i][j][k].strength_;
  }
  }
  }

  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\subpixels_";
  char buffer[10];
  fname = fname + "theta=" + itoa(theta, buffer, 10) +
  ",phi=" + itoa(phi, buffer, 10) +
  ",d=" + itoa(intd, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  for(int k=(margin+1); k<dim-(margin+1); k++)
  {
  for(int j=(margin+1); j<dim-(margin+1); j++)
  {
  for(int i=(margin+1); i<dim-(margin+1); i++)
  {
  if(res_cm[i][j][k].strength_ > maximum/2)
  {
  vcl_cout << i << ' ' << j << ' ' << k << ' ';
  vcl_cout << res_cm[i][j][k].strength_ << ' ';
  vcl_cout << res_cm[i][j][k].location_ << '\n';
  vcl_fprintf(fp, "%f %f %f\n", i+res_cm[i][j][k].location_.x(), j+res_cm[i][j][k].location_.y(), k+res_cm[i][j][k].location_.z());
  }
  }
  }
  vcl_cout << '\n';
  }
  vcl_fclose(fp);
  }
  }
  }*/

  int dim = 36;
  int kernel = 7;
  int margin = (kernel-1)/2;
  det_edge_map cm(dim, dim, dim);
  for(int radius=3; radius<=10; radius++)
  {
    for(int theta=0; theta<=90; theta=theta+5)
    {
      for(int phi=0; phi<=90; phi=phi+5)
      {
        create_3d_circle_data_analytically_v2(radius, theta, phi, cm);
        det_edge_map res_cm(suppress.apply(cm));
        double maximum = 0;
        for(int k=(margin+1); k<dim-(margin+1); k++)
        {
          for(int j=(margin+1); j<dim-(margin+1); j++)
          {
            for(int i=(margin+1); i<dim-(margin+1); i++)
            {
              if(res_cm[i][j][k].strength_ > maximum)
                maximum = res_cm[i][j][k].strength_;
            }
          }
        }
/*
        vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\subpixels_";
        char buffer[10];
        fname = fname + "radius=" + itoa(radius, buffer, 10) + ",theta=" + itoa(theta, buffer, 10) + 
          ",phi=" + itoa(phi, buffer, 10) + ".txt";
        FILE *fp = vcl_fopen(fname.data(),"w");
        for(int k=(margin+1); k<dim-(margin+1); k++)
        {
          for(int j=(margin+1); j<dim-(margin+1); j++)
          {
            for(int i=(margin+1); i<dim-(margin+1); i++)
            {
              if(res_cm[i][j][k].strength_ > maximum/2)
              {
                vcl_cout << i << ' ' << j << ' ' << k << ' ';
                vcl_cout << res_cm[i][j][k].strength_ << ' ';
                vcl_cout << res_cm[i][j][k].location_ << '\n';
                vcl_fprintf(fp, "%f %f %f\n", i+res_cm[i][j][k].location_.x(), j+res_cm[i][j][k].location_.y(),  k+res_cm[i][j][k].location_.z());
              }
            }
            vcl_cout << '\n';
          }
        }
        vcl_fclose(fp);
        */
      }
    }
  }
}

TESTMAIN(test_edge_det_nonmaximum_suppression);
