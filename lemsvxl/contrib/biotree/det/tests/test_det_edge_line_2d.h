#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_edge_map.h>
#include <det/edge_det_nonmaximum_suppression.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_erf.h>

void create_2d_line_data_analytically(int angle, int intc, det_edge_map &cm)
{
  double sigma = 1.0;
  double a = -vcl_sin(double(angle*vnl_math::pi/180));
  double b = vcl_cos(double(angle*vnl_math::pi/180));
  double c = intc / 10.0;
  // create the edge map
  const int dim = 20;
  double img[dim][dim];
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      double dist = vcl_fabs(a*x+b*y+c);
      img[x][y] = vcl_exp(-vcl_pow(dist,2.0)/(2*vcl_pow(sigma,2.0)));
    }
  }
  //write edge map to file
/*  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\img_";
  char buffer[10];
  fname = fname + "angle=" + itoa(angle, buffer, 10) + ",c=" + itoa(intc, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vcl_fprintf(fp, "%f ", img[x][y]);
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);*/

  // fill the cylinder map
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vgl_vector_3d<double> dir(a ,b, 0);
      for(int z=0;z<dim; z++)
      {
        cm[x][y][z].dir_ = dir;
        cm[x][y][z].strength_ = img[x][y];
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }
}

void create_2d_line_data_analytically_v2(int angle, int intc, det_edge_map &cm)
{
  double a = -vcl_sin(double(angle*vnl_math::pi/180));
  double b = vcl_cos(double(angle*vnl_math::pi/180));
  double c = intc / 10.0;
  // create the image
  const int dim = 26;
  double img[dim][dim];
  double grad_x[dim][dim];
  double grad_y[dim][dim];
  double erf_sigma = 1.0;
  // create image
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      double dist = a*x+b*y+c;
      img[x][y] = vnl_erf(dist / (vcl_sqrt(2.0)*erf_sigma));
//      img[x][y] = dist;
    }
  }
  // create the gaussian gradient kernels
  double sigma = 0.5;
  const int kernel = 7;
  int offset = (kernel-1) / 2;
  double gauss_x[kernel][kernel];
  double gauss_y[kernel][kernel];
  for(int x = -offset; x <= offset; x++)
  {
    for(int y = -offset; y <= offset; y++)
    {
      gauss_x[x+offset][y+offset] = (x/(vcl_pow(sigma,2.0)))*vcl_exp(-(vcl_pow(x,2.0)+vcl_pow(y,2.0))/(2*vcl_pow(sigma,2.0)))/(2*vnl_math::pi*vcl_pow(sigma,2.0));
      gauss_y[x+offset][y+offset] = (y/(vcl_pow(sigma,2.0)))*vcl_exp(-(vcl_pow(x,2.0)+vcl_pow(y,2.0))/(2*vcl_pow(sigma,2.0)))/(2*vnl_math::pi*vcl_pow(sigma,2.0));
    }
  }
  // convolve the image with the gaussian edge detector kernels
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      double val_x = 0;
      double val_y = 0;
      for(int j = -offset; j <= offset; j++)
      {
        for(int i = -offset; i <= offset; i++)
        {
          if(x+i >=0 && x+i < dim && y+j >= 0 && y+j < dim)
          {
            val_x = val_x + img[x+i][y+j] * gauss_x[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)];
            val_y = val_y + img[x+i][y+j] * gauss_y[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)];
          }
        }
      }
      grad_x[x][y] = val_x;
      grad_y[x][y] = val_y;
    }
  }
  
  // fill the cylinder map
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      for(int z=0;z<dim; z++)
      {
        vgl_vector_3d<double> dir(grad_x[x][y], grad_y[x][y], 0.0);
        cm[x][y][z].dir_ = dir;
        cm[x][y][z].strength_ = dir.length();
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }

  //write image to file
/*  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\img_";
  char buffer[10];
  fname = fname + "angle=" + itoa(angle, buffer, 10) + ",c=" + itoa(intc, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vcl_fprintf(fp, "%f ", img[x][y]);
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);*/

  //write gradient to file
/*  fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\grad_";
  fname = fname + "angle=" + itoa(angle, buffer, 10) + ",c=" + itoa(intc, buffer, 10) + ".txt";
  fp = vcl_fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vgl_vector_3d<double> dir(grad_x[x][y], grad_y[x][y], 0.0);
      vcl_fprintf(fp, "%f ", dir.length());
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);*/
}
