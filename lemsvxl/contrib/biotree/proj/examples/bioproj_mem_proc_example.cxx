//: 
// \file    bioproj_mem_proc_example.cxx
// \brief   This is the example using memory efficient processing and classes
//          User should provide a command line parameter which is the filename base for the
//          output files, e.g. if the user provides "base" as the parameter, the output files
//          will be "base_s.txt", "base_x.txt", etc.
// \author  H. Can Aras
// \date    2005-09-08
// 

#include <slice/sliceFileManager.h>
#include <proj/bioproj_mem_io.h>
#include <proj/bioproj_mem_proc.h>
#include <ctime>
#include <cstdlib>
#include <vul/vul_file.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil3d/vil3d_slice.h>

int main(int argc, char *argv[])
{
  if(argc != 5)
  {
    std::cout << "Usage: " << argv[0] << " [log name] [scan file] [box file] [sigma]\n ";
    exit(-1);
  }

  std::string outnamebase = vul_file::strip_directory(argv[2]);
  outnamebase += "_";
  outnamebase += vul_file::strip_directory(argv[3]);
  std::cout  << "outfiles will be prefixed: " << outnamebase << "\n";

  double sigma = double(std::atof(argv[4]));

  //TODO how do we determine grid spacing?
 float voxel_size = 24; 

 clock_t start, end;
 double elapsed;
 start = std::clock();

 std::string image_fname_pattern(argv[1]);
 int cut_point = image_fname_pattern.find(".log");
 image_fname_pattern.replace(cut_point, 8, "####.tif"); 

 float i_zero = 65535;
 // See the proj_io class for the description of the constructor parameters
 bioproj_mem_io proj_mem_io( argv[1], argv[2], argv[3], image_fname_pattern, 
     i_zero, voxel_size, sigma, sigma);

 bioproj_mem_proc proj_mem_proc(&proj_mem_io);

  std::string out_s = outnamebase + "_s.out";
  std::string out_x = outnamebase + "_x.out";
  std::string out_y = outnamebase + "_y.out";
  std::string out_z = outnamebase + "_z.out";



  proj_mem_proc.execute(NU_G, G  , 4);   // smoothing
  sliceFileManager<float>::write(proj_mem_io.grid_,out_s);

  proj_mem_proc.execute(NU_G_X, G, 1);     // first derivative along x
  sliceFileManager<float>::write(proj_mem_io.grid_,out_x);

  proj_mem_proc.execute(NU_G_Y, G, 1);     // first derivative along y
  sliceFileManager<float>::write(proj_mem_io.grid_,out_y);

  proj_mem_proc.execute(NU_G, G_Z, 1);     // first derivative along z
  sliceFileManager<float>::write(proj_mem_io.grid_,out_z);
  
  end = std::clock();
  elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  std::cout << "Elapsed time is " << elapsed << std::endl;

  return 0;
}
