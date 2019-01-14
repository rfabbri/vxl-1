#include <dbcvr/dbcvr_cvmatch_even.h>
#include <vul/vul_file.h>
#include <dbsol/dbsol_file_io.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>

void loadCON(std::string fileName, std::vector<vsol_point_2d_sptr> &points)
{
  std::ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (std::strncmp(magicNum,"CONTOUR",7))
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be CONTOUR " << magicNum << std::endl;
    std::exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!std::strncmp(openFlag,"OPEN",4))
    std::cout << "Open Curve\n" << std::endl;
  else if (!std::strncmp(openFlag,"CLOSE",5))
    std::cout << "Closed Curve\n" << std::endl;
  else
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be OPEN/CLOSE " << openFlag << std::endl;
    std::exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y;
  for (i=0;i<numOfPoints;i++)
  {
    infp >> x >> y;
    points.push_back(new vsol_point_2d(x, y));
  }
  infp.close();
}

void writeCON(std::string fileName, dbsol_interp_curve_2d &c, int numpoints)
{
  std::ofstream outfp(fileName.c_str());
  assert(outfp != NULL);
  outfp << "CONTOUR" << std::endl;
  outfp << "OPEN" << std::endl;
  outfp << numpoints << std::endl;
  double ds = c.length()/(numpoints-1);
  for(int i=0; i<numpoints; i++)
  {
    vsol_point_2d_sptr p = c.point_at(i*ds);
    outfp << p->x() << " " << p->y() << " " << std::endl;
  }
  outfp.close();
}

int main()
{

    std::string input1 = "/home/anilusumezbas/Desktop/curve-DP-test/geo-1.con";
    std::string input2 = "/home/anilusumezbas/Desktop/curve-DP-test/geo-2.con";

    if (!vul_file::exists(input1)) 
    {
        std::cerr << "Cannot find contour file: " << input1 << std::endl;
        return 1;
    }

    if (!vul_file::exists(input2)) 
    {
        std::cerr << "Cannot find contour file: " << input2 << std::endl;
        return 1;
    }
 
    // construct the first curve
    std::vector<vsol_point_2d_sptr> points1;
    loadCON(input1, points1);
    dbsol_interp_curve_2d curve1;
    vnl_vector<double> samples1;
    dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);

    // construct the second curve
    std::vector<vsol_point_2d_sptr> points2;
    loadCON(input2, points2);
    dbsol_interp_curve_2d curve2;
    vnl_vector<double> samples2;
    dbsol_curve_algs::interpolate_eno(&curve2,points2,samples2);

    dbcvr_cvmatch_even* DP = new dbcvr_cvmatch_even(&curve1,&curve2,curve1.size()+1,curve2.size()+1,10,3);
    DP->Match();

    std::string corr1_filename = "/home/anilusumezbas/Desktop/curve-DP-test/cvmatch_even/corr-1.txt";
    std::string corr2_filename = "/home/anilusumezbas/Desktop/curve-DP-test/cvmatch_even/corr-2.txt";

    std::ofstream corr1(corr1_filename.c_str());
    std::ofstream corr2(corr2_filename.c_str());
    
    for(unsigned i=0; i<DP->finalMapSize(); ++i)
    {
        corr1 << DP->getFMapFirst(DP->finalMapSize()-1-i) << "\n";
        corr2 << DP->getFMapSecond(DP->finalMapSize()-1-i) << "\n";
    }

    corr1.close();
    corr2.close();

    std::cout << "Final cost is: " << DP->finalCost() << std::endl;

    return 0;
}
