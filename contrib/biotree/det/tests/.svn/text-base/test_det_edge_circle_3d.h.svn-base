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

void create_3d_circle_data_analytically(int radius, int theta, int phi, det_edge_map &cm)
{
  double sigma = 1.0;
  double a = vcl_sin(double(theta*vnl_math::pi/180)) * vcl_cos(double(phi*vnl_math::pi/180));
  double b = vcl_sin(double(theta*vnl_math::pi/180)) * vcl_sin(double(phi*vnl_math::pi/180));
  double c = vcl_cos(double(theta*vnl_math::pi/180));
  double r = radius;
  vgl_point_3d<double> centre(10,10,10);
  vgl_vector_3d<double> n(a,b,c);
  // create the super resolution image
  const int dim = 20;
  double img[dim][dim][dim];
  vgl_vector_3d<double> directions[dim][dim][dim];
  for(int z=0; z<dim; z++)
  {
    for(int y=0; y<dim; y++)
    {
      for(int x=0; x<dim; x++)
      {
        vgl_point_3d<double> p(x,y,z);
        vgl_vector_3d<double> v(p-centre);
        vgl_vector_3d<double> v0(dot_product(v,n) * n);
        vgl_vector_3d<double> v1(v-v0);
        double dist = v1.length() - r;
        directions[x][y][z] = v1;
        img[x][y][z] = vcl_exp(-vcl_pow(dist,2.0)/(2*vcl_pow(sigma,2.0)));
      }
    }
  }
  //write super resolution image to file
/*  vcl_string fname = "F:\\MyDocs\\projects\\BioTree\\daily_news_2006\\jan29\\img_";
  char buffer[10];
  fname = fname + "radius=" + itoa(radius, buffer, 10) + ",center=" + itoa(theta, buffer, 10) + 
                                                         ",phi=" + itoa(phi, buffer, 10) + ".txt";
  FILE *fp = vcl_fopen(fname.data(),"w");
  
  for(int z=0; z<dim; z++)
  {
    for(int y=0; y<dim; y++)
    {
      for(int x=0; x<dim; x++)
      {
        vcl_fprintf(fp, "%f ", img[x][y][z]);
      }
      vcl_fprintf(fp, "\n");
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);*/

  // fill the edge map
  for(int z=0; z<dim; z++)
  {
    for(int y=0; y<dim; y++)
    {
      for(int x=0; x<dim; x++)
      {
        cm[x][y][z].dir_ = directions[x][y][z];
        cm[x][y][z].strength_ = img[x][y][z];
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }
}

void create_3d_circle_data_analytically_v2(int radius, int theta, int phi, det_edge_map &cm)
{
  double a = vcl_sin(double(theta*vnl_math::pi/180)) * vcl_cos(double(phi*vnl_math::pi/180));
  double b = vcl_sin(double(theta*vnl_math::pi/180)) * vcl_sin(double(phi*vnl_math::pi/180));
  double c = vcl_cos(double(theta*vnl_math::pi/180));
  double r = radius;
  vgl_point_3d<double> centre(10,10,10);
  vgl_vector_3d<double> n(a,b,c);
  // create the image
  const int dim = 36;
  double img[dim][dim][dim];
  double grad_x[dim][dim][dim];
  double grad_y[dim][dim][dim];
  double grad_z[dim][dim][dim];
  double erf_sigma = 1.0;
  // create image
  for(int z=0; z<dim; z++)
  {
    for(int y=0; y<dim; y++)
    {
      for(int x=0; x<dim; x++)
      {
        vgl_point_3d<double> p(x,y,z);
        vgl_vector_3d<double> v(p-centre);
        vgl_vector_3d<double> v0(dot_product(v,n) * n);
        vgl_vector_3d<double> v1(v-v0);
        double dist = v1.length() - r;
        img[x][y][z] = vnl_erf(dist / (vcl_sqrt(2.0)*erf_sigma));
      }
    }
  }
  // create the gaussian gradient kernels
  double sigma = 0.5;
  const int kernel = 7;
  int offset = (kernel-1) / 2;
  double gauss_x[kernel][kernel][kernel];
  double gauss_y[kernel][kernel][kernel];
  double gauss_z[kernel][kernel][kernel];
  for(int x = -offset; x <= offset; x++)
  {
    for(int y = -offset; y <= offset; y++)
    {
      for(int z = -offset; z <= offset; z++)
      {
        gauss_x[x+offset][y+offset][z+offset] = (x/(vcl_pow(sigma,2.0)))
          *vcl_exp(-(vcl_pow(x,2.0)+vcl_pow(y,2.0)+vcl_pow(z,2.0))/(2*vcl_pow(sigma,2.0)))
          /(vcl_pow(sqrt(2*vnl_math::pi)*sigma,3.0));
        gauss_y[x+offset][y+offset][z+offset] = (y/(vcl_pow(sigma,2.0)))
          *vcl_exp(-(vcl_pow(x,2.0)+vcl_pow(y,2.0)+vcl_pow(z,2.0))/(2*vcl_pow(sigma,2.0)))
          /(vcl_pow(sqrt(2*vnl_math::pi)*sigma,3.0));
        gauss_z[x+offset][y+offset][z+offset] = (z/(vcl_pow(sigma,2.0)))
          *vcl_exp(-(vcl_pow(x,2.0)+vcl_pow(y,2.0)+vcl_pow(z,2.0))/(2*vcl_pow(sigma,2.0)))
          /(vcl_pow(sqrt(2*vnl_math::pi)*sigma,3.0));
      }
    }
  }
  // convolve the image with the gaussian edge detector kernels
  for(int z=0; z<dim; z++)
  {
    for(int y=0; y<dim; y++)
    {
      for(int x=0; x<dim; x++)
      {
        double val_x = 0;
        double val_y = 0;
        double val_z = 0;
        for(int k = -offset; k <= offset; k++)
        {
          for(int j = -offset; j <= offset; j++)
          {
            for(int i = -offset; i <= offset; i++)
            {
              if(x+i >=0 && x+i < dim && y+j >= 0 && y+j < dim && z+k >= 0 && z+k < dim)
              {
                val_x = val_x + img[x+i][y+j][z+k] * gauss_x[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
                val_y = val_y + img[x+i][y+j][z+k] * gauss_y[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
                val_z = val_z + img[x+i][y+j][z+k] * gauss_z[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
              }
            }
          }
        }
        grad_x[x][y][z] = val_x;
        grad_y[x][y][z] = val_y;
        grad_z[x][y][z] = val_z;
      }
    }
  }
  
  // fill the edge map
  for(int y=0; y<dim; y++)
  {
    for(int x=0; x<dim; x++)
    {
      for(int z=0;z<dim; z++)
      {
        vgl_vector_3d<double> dir(grad_x[x][y][z], grad_y[x][y][z], grad_z[x][y][z]);
        cm[x][y][z].dir_ = dir;
        cm[x][y][z].strength_ = dir.length();
        cm[x][y][z].location_ = vgl_point_3d<double> (0.0,0.0,0.0);
      }
    }
  }
}

