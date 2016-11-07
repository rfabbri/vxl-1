//: 
// \file     vol3d_synthetic_cylinder_generator.cxx
// \brief    generates 3D volume data by using an input file. Input file lists
//           the cylinders as:
//           c (276.75, 100.0, 276.75) 50 400 (0.707, 0, .707)
//           c (center) rad length (direction)
//           The dimensions of the 3D box is as follows:
//           d (70, 20, 70) 10.0
//           d (xdim ydim zdim) voxel_size
//
//           Argument List:
//           -i (input file) -o (ouput file (gipl)) -t (output txt file (opt))
//
// \author   Gamze D. Tunali
// \date     May 20, 2006
// 
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_cylinder.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vol3d/vol3d_gaussian_kernel_3d.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>

//: reads the input file and creates a list of cylinders to be filled in
// the voxels
void create_cyls(vcl_string fname, vcl_vector<vsol_cylinder> &cylinders,
                 int &xdim, int &ydim, int &zdim, 
                 double &voxel_size)
{
  vcl_FILE *fp;
  fp = vcl_fopen(fname.data(), "r");
  assert(fp != NULL);
  char dummy[80];
  vcl_fscanf(fp, "%s\n", dummy);
  assert(!vcl_strcmp(dummy, "synthetic_cylinder_generator_input"));

  double posx, posy, posz;
  double orx, ory, orz;
  double radius, length;
  char start_of_line;
  char check;
  int counter = 0;
  while(1)
  {
    vcl_fscanf(fp, "%c", &start_of_line);
    if(start_of_line == 'e')
      break;
    else if(start_of_line == 'c') //this line is a cylinder input
    {
      vcl_fscanf(fp, " %c", &check);
      assert(check == '(');
      vcl_fscanf(fp, "%lf%c", &posx, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &posy, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &posz, &check);
      assert(check == ')');
      vcl_fscanf(fp, "%lf%lf", &radius, &length);
      vcl_fscanf(fp, " %c", &check);
      assert(check == '(');
      vcl_fscanf(fp, "%lf%c", &orx, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &ory, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c\n", &orz, &check);
      assert(check == ')');

      vgl_point_3d<double> centre(posx, posy, posz);
      double l = vcl_sqrt((orx*orx) + (ory*ory) + (orz*orz));
      vgl_vector_3d<double> orientation(orx/l, ory/l, orz/l);

      vsol_cylinder cyl(centre, radius, length);
      cyl.set_orientation(orientation);
      vcl_cout << cyl << vcl_endl;
      cylinders.push_back(cyl);
      
    }
    else if(start_of_line == 'd') //this line is for the dimensions of the box
    {
      vcl_fscanf(fp, " %c", &check);
      assert(check == '(');
      vcl_fscanf(fp, "%d%c", &xdim, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%d%c", &ydim, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%d%c", &zdim, &check);
      assert(check == ')');
      vcl_fscanf(fp, "%lf", &voxel_size);
    }
  }
  fclose(fp);
}

//: creates the 3D data by filling out the voxels based on the "inside cylinder"
// test. 
vbl_array_3d<double> create_3d_data(vcl_string fname, vcl_vector<vsol_cylinder> objs, 
                           int xdim, int ydim, int zdim, double voxel_size)
{
  vil3d_image_view<unsigned char> view(xdim, ydim, zdim);
  vbl_array_3d<double> voxels(xdim, ydim, zdim);
  
  for (int i=0; i<zdim; i++){
    for (int j=0; j<ydim; j++){
      for (int k=0; k<xdim; k++){
        unsigned char &c = view(k, j, i);
        double &v = voxels(k, j, i);
        bool on=false;
        for (unsigned num=0; num<objs.size(); num++) {
          if (objs[num].contains(vgl_point_3d<double> (k*voxel_size, j*voxel_size, i*voxel_size))){
            on = true;
          }
        }
        if (on) {
          v = 255.0;
          c = 255;
        } else { 
          v = 0;
          c = 0;
      }
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
  for (int i=0; i<zdim; i++){
    of << "z=" << i << vcl_endl;
      for (int j=0; j<ydim; j++){
      of << "y=" << j << " ";
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
  vcl_vector<vsol_cylinder> cylinders;
  double voxel_size;
  int xdim, ydim, zdim;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);
    if (arg == vcl_string ("-i")) { in_fname = vcl_string(argv[++i]);}
    else if (arg == vcl_string ("-o")) {out_fname = vcl_string(argv[++i]);}
  else if (arg == vcl_string ("-t")) {txt_fname = vcl_string(argv[++i]);}
    else
    {
      vcl_cout << "Usage: " << argv[0] << "[-i cylinders input file][-o output file][-t txt_file (opt)] " << vcl_endl;
      throw -1;
    }
  }

  if ((in_fname == "") || (out_fname == "")) {
    vcl_cerr << "You should provide and input and output file paths" << vcl_endl;
    vcl_cout << "Usage: " << argv[0] << "[-i cylinders input file][-o output file][-t txt_file (opt)] " << vcl_endl;
      return 1;
  }

  create_cyls(in_fname, cylinders, xdim, ydim, zdim, voxel_size);
  vbl_array_3d<double> vol = create_3d_data(out_fname, cylinders, xdim, ydim, zdim, voxel_size);
  if (txt_fname == "") 
    return 1;

  vcl_ofstream of(txt_fname.data());
  print_volume(of, vol, xdim, ydim, zdim);
  
}
