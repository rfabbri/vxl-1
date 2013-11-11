#include <dbcvr/dbcvr_cvmatch.h>
#include <vul/vul_file.h>
#include <dbsol/dbsol_file_io.h>
#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <vcl_fstream.h>

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

    vcl_vector<vsol_point_2d_sptr> points1, points2;
    bool isClosed1, isClosed2;

    if(!dbsol_load_con_file(input1.c_str(),points1,isClosed1))
    {
        vcl_cerr << "Problems with loading the first con file" << vcl_endl;
        return 1;
    }
    if(!dbsol_load_con_file(input2.c_str(),points2,isClosed2))
    {
        vcl_cerr << "Problems with loading the second con file" << vcl_endl;
        return 1;
    }

    bsol_intrinsic_curve_2d_sptr curve1=new bsol_intrinsic_curve_2d(points1);
    bsol_intrinsic_curve_2d_sptr curve2=new bsol_intrinsic_curve_2d(points2);

    dbcvr_cvmatch* DP = new dbcvr_cvmatch();
    DP->setCurve1(curve1);
    DP->setCurve2(curve2);

    DP->Match();

    vcl_string corr1_filename = "/home/anilusumezbas/Desktop/curve-DP-test/corr-1.txt";
    vcl_string corr2_filename = "/home/anilusumezbas/Desktop/curve-DP-test/corr-2.txt";

    vcl_ofstream corr1(corr1_filename.c_str());
    vcl_ofstream corr2(corr2_filename.c_str());
    
    for(unsigned i=0; i<DP->finalMapSize(); ++i)
    {
        corr1 << DP->getFMapFirst(DP->finalMapSize()-i) << "\n";
        corr2 << DP->getFMapSecond(DP->finalMapSize()-i) << "\n";
    }

    corr1.close();
    corr2.close();

    vcl_cout << "Final cost is: " << DP->finalCost() << vcl_endl;

    return 1;
}
