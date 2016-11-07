//: 
// \file    bioproj_proc_example.cxx
// \brief   User should provide a command line parameter which is the filename base for the
//          output files, e.g. if the user provides "base" as the parameter, the output files
//          will be "base_s.txt", "base_x.txt", etc.
// \author  H. Can Aras
// \date    2005-07-13
// 

#include <proj/bioproj_io.h>
#include <proj/bioproj_proc.h>
#include <vcl_ctime.h>

int main(int argc, char *argv[])
{
  if(argc != 5)
  {
    vcl_cout << "Usage: " << argv[0] << " [filename base]" << vcl_endl;
    exit(-1);
  }
    
  vcl_string fnamebase = argv[1];

  clock_t start, end;
  double elapsed;
  start = vcl_clock();

  double sigma = double(atof(argv[2]));
  int r_extent = atoi(argv[3]);
  int z_extent = atoi(argv[4]);

  bioproj_io proj_io("newcastfragment35um2.scn", 
    "newcast35um2_curvy_roi_5x5x3.bx3",
    35.42582, 46.8, 261.5, 345.712, vnl_int_2(590, 524), 0.9,
    "/home/jdfunaro/newcast35um2/scan35um####.tif",
    50.0, 50.0, 10, 5, 10);

  bioproj_proc proj_proc(&proj_io);

  vcl_string fs = fnamebase + "_s.txt";
  vcl_string fx = fnamebase + "_x.txt";
  vcl_string fy = fnamebase + "_y.txt";
  vcl_string fz = fnamebase + "_z.txt";

  proj_proc.execute(NU_G, G, fs.c_str());       // smoothing

  proj_proc.execute(NU_G_X, G, fx.c_str());     // first derivative along x
  proj_proc.execute(NU_G_Y, G, fy.c_str());     // first derivative along y
  proj_proc.execute(NU_G, G_Z, fz.c_str());     // first derivative along z
  
  /*
  proj_proc.execute(NU_G_XX, G, fxx.c_str());   // second derivative xx
  proj_proc.execute(NU_G_YY, G, fyy.c_str());   // second derivative yy
  proj_proc.execute(NU_G_XY, G, fxy.c_str());   // second derivative xy

  proj_proc.execute(NU_G_X, G_Z , fxz.c_str()); // second derivative xz
  proj_proc.execute(NU_G_Y, G_Z , fyz.c_str()); // second derivative yz
  proj_proc.execute(NU_G  , G_ZZ, fzz.c_str()); // second derivative zz
*/
  end = vcl_clock();
  elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  vcl_cout << "Elapsed time is " << elapsed << vcl_endl;

  return 0;
}
