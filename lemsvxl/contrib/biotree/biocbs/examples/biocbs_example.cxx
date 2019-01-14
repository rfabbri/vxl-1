//: 
// \file    biocbs_example.cxx
// \brief   
// \author  H. Can Aras
// \date    2006-07-26
// 

#include <biocbs/biocbs_scan.h>
#include <vbl/vbl_array_3d.h>
#include <cstdio>

int main(int argc, char *argv[])
{
  int dimx, dimy, dimz;
  int numviews = 400;
  unsigned short fill_value = 0;
#if 0
  int val;
  FILE *fp = std::fopen("D:\\MyDocs\\projects\\BioTree\\daily_news_2006\\aug1_volumetric_vs_projected\\straight_big\\vol.txt", "r");
  std::fscanf(fp, "%d %d %d\n", &dimx, &dimy, &dimz);

  vbl_array_3d<unsigned short> vol(dimx, dimy, dimz, fill_value);
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        std::fscanf(fp,"%d", &val);
        vol(i,j,k) = (unsigned short)(val);
      }
    }
  }
  std::fclose(fp);
#endif

// sphere test
#if 1
  double val;
  dimx = 51;
  dimy = 51;
  dimz = 51;

  FILE *fp = std::fopen("D:\\MyDocs\\Temp\\sphere_vol.txt", "w");
  std::fprintf(fp, "%d %d %d\n", dimx, dimy, dimz);

  int x0 = 25;
  int y0 = 25;
  int z0 = 25;
  int r = 10;
  vbl_array_3d<unsigned short> vol(dimx, dimy, dimz, fill_value);
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        val = std::pow((i-x0),2.0) + std::pow((j-y0),2.0) + std::pow((k-z0),2.0);
        if(val <= std::pow(r,2.0))
          vol(i,j,k) = 1;
        std::fprintf(fp, "%d ", vol(i,j,k));
      }
      std::fprintf(fp, "\n");
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);
#endif

  biocbs_scan cbs(46.8, 261.5, 345.712, vnl_int_2(590, 524),
    vnl_double_2(294.5, 267.62), vgl_point_3d<double> (-0.18772, -262.064, 0.0),
    vnl_double_3(0.0, 0.0, 1.0), 2*vnl_math::pi/numviews, numviews,
    35.42582, &vol, vgl_point_3d<double> (5.8452603, 0.28340656, 4.066328315), NO_INTERP);

  cbs.write_scan("D:\\MyDocs\\Temp\\scan.scn");
  cbs.write_box("D:\\MyDocs\\Temp\\box.bx3");

  for(int i=0; i < numviews; i++)
  {
    std::cout << i << std::endl;
    cbs.project_volume(i);
  }

//  cbs.adjust_data_range(65535);
//  cbs.write_data_3d("D:\\MyDocs\\projects\\BioTree\\read_volume_file\\vol.txt");

  cbs.write_data_2d_views("D:\\MyDocs\\Temp\\images\\img");

  return 0;
}
