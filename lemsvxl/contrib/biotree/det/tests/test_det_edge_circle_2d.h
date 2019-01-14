#include <testlib/testlib_test.h>
#include <iostream>
#include <det/det_edge_map.h>
#include <det/edge_det_nonmaximum_suppression.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_erf.h>

void create_2d_circle_data_analytically(int radius, int centre_x , int centre_y, det_edge_map &cm)
{
  double sigma = 1.0;
  int r = radius;
  int xc = centre_x;
  int yc = centre_y;
  // create the super resolution image
  const int dim = 26;
  double img[dim][dim];
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      double dist = std::sqrt(std::pow(x-xc,2.0) + std::pow(y-yc,2.0)) - r;
      img[x][y] = std::exp(-std::pow(dist,2.0)/(2*std::pow(sigma,2.0)));
    }
  }
  //write super resolution image to file
/*  std::string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\img_";
  char buffer[10];
  fname = fname + "radius=" + itoa(radius, buffer, 10) + ",center=" + itoa(centre_x, buffer, 10) + 
                                                         "," + itoa(centre_y, buffer, 10) + ".txt";
  FILE *fp = std::fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      std::fprintf(fp, "%f ", img[x][y]);
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);*/

  // fill the cylinder map
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vgl_vector_3d<double> dir(x-xc, y-yc, 0);
      for(int z=0;z<dim; z++)
      {
        cm[x][y][z].dir_ = dir;
        cm[x][y][z].strength_ = img[x][y];
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }
}

void create_2d_circle_data_analytically_v2(int radius, int centre_x , int centre_y, det_edge_map &cm)
{
  int r = radius;
  int xc = centre_x;
  int yc = centre_y;
  // create the image
  // create the image
  const int dim = 26;
  double img[dim][dim];
  double grad_x[dim][dim];
  double grad_y[dim][dim];
  double erf_sigma = 1.0;
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      double dist = std::sqrt(std::pow(x-xc,2.0) + std::pow(y-yc,2.0)) - r;
      img[x][y] = vnl_erf(dist / (std::sqrt(2.0)*erf_sigma));
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
      gauss_x[x+offset][y+offset] = (x/(std::pow(sigma,2.0)))*std::exp(-(std::pow(x,2.0)+std::pow(y,2.0))/(2*std::pow(sigma,2.0)))/(2*vnl_math::pi*std::pow(sigma,2.0));
      gauss_y[x+offset][y+offset] = (y/(std::pow(sigma,2.0)))*std::exp(-(std::pow(x,2.0)+std::pow(y,2.0))/(2*std::pow(sigma,2.0)))/(2*vnl_math::pi*std::pow(sigma,2.0));
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
//        vgl_vector_3d<double> dir2(a ,b, 0);
        cm[x][y][z].dir_ = dir;
        cm[x][y][z].strength_ = dir.length();
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }

  //write image to file
/*  std::string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\img_";
  char buffer[10];
  fname = fname + "radius=" + itoa(radius, buffer, 10) + ",center=" + itoa(centre_x, buffer, 10) + 
                                                         "," + itoa(centre_y, buffer, 10) + ".txt";
  FILE *fp = std::fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      std::fprintf(fp, "%f ", img[x][y]);
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);*/

  //write gradient to file
/*  fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\grad_";
  fname = fname + "radius=" + itoa(radius, buffer, 10) + ",center=" + itoa(centre_x, buffer, 10) + 
                                                         "," + itoa(centre_y, buffer, 10) + ".txt";
  fp = std::fopen(fname.data(),"w");
  
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      vgl_vector_3d<double> dir(grad_x[x][y], grad_y[x][y], 0.0);
      std::fprintf(fp, "%f ", dir.length());
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);*/
}
