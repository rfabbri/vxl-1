#include <dbcvr/dbcvr_cvmatch_even.h>
#include <vul/vul_file.h>
#include <dbsol/dbsol_file_io.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_fstream.h>

void loadCON(vcl_string fileName, vcl_vector<vsol_point_2d_sptr> &points)
{
  vcl_ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (vcl_strncmp(magicNum,"CONTOUR",7))
  {
    vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cerr << "Should be CONTOUR " << magicNum << vcl_endl;
    vcl_exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!vcl_strncmp(openFlag,"OPEN",4))
    vcl_cout << "Open Curve\n" << vcl_endl;
  else if (!vcl_strncmp(openFlag,"CLOSE",5))
    vcl_cout << "Closed Curve\n" << vcl_endl;
  else
  {
    vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cerr << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    vcl_exit(1);
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

void writeCON(vcl_string fileName, dbsol_interp_curve_2d &c, int numpoints)
{
  vcl_ofstream outfp(fileName.c_str());
  assert(outfp != NULL);
  outfp << "CONTOUR" << vcl_endl;
  outfp << "OPEN" << vcl_endl;
  outfp << numpoints << vcl_endl;
  double ds = c.length()/(numpoints-1);
  for(int i=0; i<numpoints; i++)
  {
    vsol_point_2d_sptr p = c.point_at(i*ds);
    outfp << p->x() << " " << p->y() << " " << vcl_endl;
  }
  outfp.close();
}

int main()
{

    vcl_string input1 = "/home/anilusumezbas/Desktop/curve-DP-test/geo-1.con";
    vcl_string input2 = "/home/anilusumezbas/Desktop/curve-DP-test/geo-2.con";

    if (!vul_file::exists(input1)) 
    {
        vcl_cerr << "Cannot find contour file: " << input1 << vcl_endl;
        return 1;
    }

    if (!vul_file::exists(input2)) 
    {
        vcl_cerr << "Cannot find contour file: " << input2 << vcl_endl;
        return 1;
    }
 
    // construct the first curve
    vcl_vector<vsol_point_2d_sptr> points1;
    loadCON(input1, points1);
    dbsol_interp_curve_2d curve1;
    vnl_vector<double> samples1;
    dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);

    // construct the second curve
    vcl_vector<vsol_point_2d_sptr> points2;
    loadCON(input2, points2);
    dbsol_interp_curve_2d curve2;
    vnl_vector<double> samples2;
    dbsol_curve_algs::interpolate_eno(&curve2,points2,samples2);

    dbcvr_cvmatch_even* DP = new dbcvr_cvmatch_even(&curve1,&curve2,curve1.size()+1,curve2.size()+1,10,3);
    DP->Match();

    vcl_string corr1_filename = "/home/anilusumezbas/Desktop/curve-DP-test/cvmatch_even/corr-1.txt";
    vcl_string corr2_filename = "/home/anilusumezbas/Desktop/curve-DP-test/cvmatch_even/corr-2.txt";

    vcl_ofstream corr1(corr1_filename.c_str());
    vcl_ofstream corr2(corr2_filename.c_str());
    
    for(unsigned i=0; i<DP->finalMapSize(); ++i)
    {
        corr1 << DP->getFMapFirst(DP->finalMapSize()-1-i) << "\n";
        corr2 << DP->getFMapSecond(DP->finalMapSize()-1-i) << "\n";
    }

    corr1.close();
    corr2.close();

    vcl_cout << "Final cost is: " << DP->finalCost() << vcl_endl;

    return 0;
}
