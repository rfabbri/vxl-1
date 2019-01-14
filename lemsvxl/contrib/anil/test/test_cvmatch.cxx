#include <dbcvr/dbcvr_cvmatch.h>
#include <vul/vul_file.h>
#include <dbsol/dbsol_file_io.h>
#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <fstream>

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

    std::vector<vsol_point_2d_sptr> points1, points2;
    bool isClosed1, isClosed2;

    if(!dbsol_load_con_file(input1.c_str(),points1,isClosed1))
    {
        std::cerr << "Problems with loading the first con file" << std::endl;
        return 1;
    }
    if(!dbsol_load_con_file(input2.c_str(),points2,isClosed2))
    {
        std::cerr << "Problems with loading the second con file" << std::endl;
        return 1;
    }

    bsol_intrinsic_curve_2d_sptr curve1=new bsol_intrinsic_curve_2d(points1);
    bsol_intrinsic_curve_2d_sptr curve2=new bsol_intrinsic_curve_2d(points2);

    dbcvr_cvmatch* DP = new dbcvr_cvmatch();
    DP->setCurve1(curve1);
    DP->setCurve2(curve2);

    DP->Match();

    std::string corr1_filename = "/home/anilusumezbas/Desktop/curve-DP-test/corr-1.txt";
    std::string corr2_filename = "/home/anilusumezbas/Desktop/curve-DP-test/corr-2.txt";

    std::ofstream corr1(corr1_filename.c_str());
    std::ofstream corr2(corr2_filename.c_str());
    
    for(unsigned i=0; i<DP->finalMapSize(); ++i)
    {
        corr1 << DP->getFMapFirst(DP->finalMapSize()-i) << "\n";
        corr2 << DP->getFMapSecond(DP->finalMapSize()-i) << "\n";
    }

    corr1.close();
    corr2.close();

    std::cout << "Final cost is: " << DP->finalCost() << std::endl;

    return 1;
}
