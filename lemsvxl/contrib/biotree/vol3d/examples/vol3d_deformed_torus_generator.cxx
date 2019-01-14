//: 
// \file     vol3d_deformed_torus_generator.cxx
// \brief    generates 3D deformed torus voxel data. Saves the volume in a .gipl file.
//
// \author   Gamze D. Tunali
// \date     AUg 12, 2006
// 
#include <iostream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_math.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_image_view.h>

#include <biosim/biosim_deformed_torus.h>


//: creates the 3D data by filling out the voxels based on the "inside torus"
// computation. 
vbl_array_3d<double> create_3d_data(std::string fname, 
                  double a, double b, 
                  double u_step, double v_step,
                  double r_step, double c,
                  int xdim, int ydim, int zdim, 
                  double voxel_size)
{
  vil3d_image_view<unsigned char> view(xdim, ydim, zdim);
  view.fill(0);
  vbl_array_3d<double> voxels(xdim, ydim, zdim);
  voxels.fill(0.0);
  double x, y, z;
  int x_pos, y_pos, z_pos;

  for (double u=0; u < (2.0 * vnl_math::pi); u += u_step){
    for (double r=0; r <= (a + b*std::cos(u)); r += r_step){
    for (double v=0; v < (2.0 * vnl_math::pi); v += v_step){
    x = (c + (r * std::cos(v))) * std::cos(u);
    y = (c + (r * std::cos(v))) * std::sin(u);
    z = r * std::sin(v);  
    x_pos = static_cast<int> (x + xdim/2.0);
    y_pos = static_cast<int> (y + ydim/2.0);
    z_pos = static_cast<int> (z + zdim/2.0);
        unsigned char &voxel = view(x_pos, y_pos, z_pos);
        double &vx = voxels(x_pos, y_pos, z_pos);
        vx = 1;
        voxel = 255;
    }
  }
  }

  // write the voxels
  vil3d_save(view, fname.data());
  return voxels;
}

void print_volume(std::ofstream& of, vbl_array_3d<double> vol,
          int xdim, int ydim, int zdim)
{
  of << xdim << " " << ydim << " " << zdim << std::endl;
  for (int i=0; i<zdim; i++){
    //of << "z=" << i << std::endl;
      for (int j=0; j<ydim; j++){
      //of << "y=" << j << " ";
        for (int k=0; k<xdim; k++){
      of << vol[k][j][i] << " ";
    }
    of << std::endl;
    }
    of << std::endl;
  }
}

int main(int argc, char* argv[])
{
  std::string in_fname="", out_fname="", txt_fname="";
  int xdim=100, ydim=100, zdim=60;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    std::string arg (argv[i]);
    if (arg == std::string ("-o")) {out_fname = std::string(argv[++i]);}
  else if (arg == std::string ("-t")) {txt_fname = std::string(argv[++i]);}
    else
    {
      std::cout << "Usage: " << argv[0] << "[-o output file][-t txt_file (opt)] " << std::endl;
      throw -1;
    }
  }

  if ((out_fname == "")) {
    std::cerr << "You should provide an output file path" << std::endl;
    std::cout << "Usage: " << argv[0] << "[-o output file][-t txt_file (opt)] " << std::endl;
      return 1;
  }

  // this method was discrete volume generatir, not using it now..
  /*vbl_array_3d<double> vol = create_3d_data(out_fname, a, b, u_step, v_step, r_step, 
  c, xdim, ydim, zdim, 1);*/

  biosim_deformed_torus torus(xdim, ydim, zdim);
  vbl_array_3d<double> volume = torus.volume();
  vil3d_image_view<unsigned char> view = torus.view();
  vil3d_save(view, out_fname.data());

  if (txt_fname == "") 
    return 1;

  std::ofstream of(txt_fname.data());
  print_volume(of, volume, xdim, ydim, zdim);
  
}
