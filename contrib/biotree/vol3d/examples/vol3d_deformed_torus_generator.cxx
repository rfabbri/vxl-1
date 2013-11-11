//: 
// \file     vol3d_deformed_torus_generator.cxx
// \brief    generates 3D deformed torus voxel data. Saves the volume in a .gipl file.
//
// \author   Gamze D. Tunali
// \date     AUg 12, 2006
// 
#include <vcl_iostream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_math.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_image_view.h>

#include <biosim/biosim_deformed_torus.h>


//: creates the 3D data by filling out the voxels based on the "inside torus"
// computation. 
vbl_array_3d<double> create_3d_data(vcl_string fname, 
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
    for (double r=0; r <= (a + b*vcl_cos(u)); r += r_step){
    for (double v=0; v < (2.0 * vnl_math::pi); v += v_step){
    x = (c + (r * vcl_cos(v))) * vcl_cos(u);
    y = (c + (r * vcl_cos(v))) * vcl_sin(u);
    z = r * vcl_sin(v);  
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

void print_volume(vcl_ofstream& of, vbl_array_3d<double> vol,
          int xdim, int ydim, int zdim)
{
  of << xdim << " " << ydim << " " << zdim << vcl_endl;
  for (int i=0; i<zdim; i++){
    //of << "z=" << i << vcl_endl;
      for (int j=0; j<ydim; j++){
      //of << "y=" << j << " ";
        for (int k=0; k<xdim; k++){
      of << vol[k][j][i] << " ";
    }
    of << vcl_endl;
    }
    of << vcl_endl;
  }
}

int main(int argc, char* argv[])
{
  vcl_string in_fname="", out_fname="", txt_fname="";
  int xdim=100, ydim=100, zdim=60;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);
    if (arg == vcl_string ("-o")) {out_fname = vcl_string(argv[++i]);}
  else if (arg == vcl_string ("-t")) {txt_fname = vcl_string(argv[++i]);}
    else
    {
      vcl_cout << "Usage: " << argv[0] << "[-o output file][-t txt_file (opt)] " << vcl_endl;
      throw -1;
    }
  }

  if ((out_fname == "")) {
    vcl_cerr << "You should provide an output file path" << vcl_endl;
    vcl_cout << "Usage: " << argv[0] << "[-o output file][-t txt_file (opt)] " << vcl_endl;
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

  vcl_ofstream of(txt_fname.data());
  print_volume(of, volume, xdim, ydim, zdim);
  
}
