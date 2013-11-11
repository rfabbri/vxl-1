//: 
// \file    det_edge_nonmax_supp_example_from_gradient.cxx
//          This example is to demonstrate how to use 3D nonmax suppression for edges
//          using the gradients in x,y and z
//          Image can be any format that vil library reads
// \brief  
// \author  Can Aras (can@lems.brown.edu)
// \date    2006-01-26
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_new.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_edge_nonmaximum_suppression.h>
#include <proc/io/proc_io_filter_xml_parser.h>
#include <vsol/vsol_cylinder.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

int main(int argc, char *argv[])
{
  if(argc < 9){
    vcl_cout << "Usage: "<< argv[0] << "filename_base dimx dimy num_files xmargin ymargin zmargin out_file\n";
    return 1;
  }

  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
//  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // initialize bgui_3d
//  bgui3d_init();

  vcl_string filename_base = argv[1];
  int dimx = atoi(argv[2]);
  int dimy = atoi(argv[3]);
  int dimz = atoi(argv[4]);
  int marginx = atoi(argv[5]);
  int marginy = atoi(argv[6]);
  int marginz = atoi(argv[7]);
  char* o_file = argv[8];
 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;

  // allocate space for the image stack (or volume) and gradients
  unsigned short ***vol;
  double ***grad_x;
  double ***grad_y;
  double ***grad_z;
  vol = new unsigned short**[dimx];
  grad_x = new double**[dimx];
  grad_y = new double**[dimx];
  grad_z = new double**[dimx];
  for(int i=0;i<dimx;i++)
  {
    vol[i] = new unsigned short*[dimy];
    grad_x[i] = new double*[dimy];
    grad_y[i] = new double*[dimy];
    grad_z[i] = new double*[dimy];
  }
  for(int i=0;i<dimx;i++)
  {
    for(int j=0;j<dimy;j++)
    {
      vol[i][j] = new unsigned short[dimz];
      grad_x[i][j] = new double[dimz];
      grad_y[i][j] = new double[dimz];
      grad_z[i][j] = new double[dimz];
    }
  }
  // read the stack of images and fill the volume
  for(int k=0; k<dimz; k++)
  {
    char buffer[1024];
    // if the images are of different format, change the sprintf below
    vcl_sprintf(buffer, "%s%03d.tif",filename_base.data(), k);
    vcl_cout << buffer << vcl_endl;
    vil_image_view<vil_rgb<vxl_byte> > img;
    vil_image_view<vxl_byte> grey_img;
    img = vil_convert_to_component_order( vil_convert_to_n_planes(3,vil_convert_cast(vxl_byte(),vil_load(buffer))));
    vil_convert_rgb_to_grey(img, grey_img);
    for(int i=0; i<dimx; i++)
      for(int j=0; j<dimy; j++)
        vol[i][j][k] = grey_img(i,j);
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

  // convolve the image with the Gaussian edge detector kernels
  for(int z=0; z<dimz; z++)
  {
    vcl_cout << "processing slice " << z+1 << " of " << dimz << " slices for gradient" << vcl_endl;
    for(int y=0; y<dimy; y++)
    {
      for(int x=0; x<dimx; x++)
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
              if(x+i >=0 && x+i < dimx && y+j >= 0 && y+j < dimy && z+k >= 0 && z+k < dimz)
              {
                val_x = val_x + vol[x+i][y+j][z+k] * gauss_x[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
                val_y = val_y + vol[x+i][y+j][z+k] * gauss_y[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
                val_z = val_z + vol[x+i][y+j][z+k] * gauss_z[(kernel-1)-(i+offset)][(kernel-1)-(j+offset)][(kernel-1)-(k+offset)];
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

  /*FILE *fp = vcl_fopen("F:\\MyDocs\\Temp\\imgx.pgm","w");
  vcl_fprintf(fp,"P2\n%d %d\n%d\n", dimx, dimy, 255);
  for(int j=0; j<dimy; j++)
  {
    for(int i=0; i<dimx; i++)
    {
      vcl_fprintf(fp,"%d ", int(vcl_abs(grad_x[i][j][10])));
    }
    vcl_fprintf(fp,"\n");
  }
  vcl_fclose(fp);

  fp = vcl_fopen("F:\\MyDocs\\Temp\\imgy.pgm","w");
  vcl_fprintf(fp,"P2\n%d %d\n%d\n", dimx, dimy, 255);
  for(int j=0; j<dimy; j++)
  {
    for(int i=0; i<dimx; i++)
    {
      vcl_fprintf(fp,"%d ", int(vcl_abs(grad_y[i][j][10])));
    }
    vcl_fprintf(fp,"\n");
  }
  vcl_fclose(fp);

  fp = vcl_fopen("F:\\MyDocs\\Temp\\imgz.pgm","w");
  vcl_fprintf(fp,"P2\n%d %d\n%d\n", dimx, dimy, 255);
  for(int j=0; j<dimy; j++)
  {
    for(int i=0; i<dimx; i++)
    {
      vcl_fprintf(fp,"%d ", int(vcl_abs(grad_x[i][j][10])));
    }
    vcl_fprintf(fp,"\n");
  }
  vcl_fclose(fp);*/

  det_edge_map cm(dimx, dimy, dimz, grad_x, grad_y, grad_z);
  cm = cm.nonmaxium_suppression_for_edge_detection();

  double maximum_strength = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        intensity = cm[i][j][k].strength_;
        if(intensity > maximum_strength)
          maximum_strength = intensity;
      }
    }
  }

  double threshold = maximum_strength / 3;
  FILE *fp = vcl_fopen(o_file, "w");
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        if(cm[i][j][k].strength_ != 0)
          vcl_fprintf(fp, "%d %d %d %f %f %f %f\n",
          i, j, k, cm[i][j][k].location_.x(), cm[i][j][k].location_.y(), cm[i][j][k].location_.z(), cm[i][j][k].strength_);
      }
    }
  }
  vcl_fclose(fp);
  return 0;
}
