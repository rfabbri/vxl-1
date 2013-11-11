#include "dbcvr_cvmatch_intensity.h"
#include <vcl_fstream.h>

int main()
{

    vcl_vector<double> vector1(50,0);
    vcl_vector<double> vector2(50,0);

    for(unsigned i=0; i<50; ++i)
    {
        if(i>=0 && i<10)
            vector1[i] = 100;
        else if(i>=10 && i<20)
            vector1[i] = 200;
        else if(i>=20 && i<35)
            vector1[i] = 50;
        else if(i>=35 && i<45)
            vector1[i] = 150;
        else
            vector1[i] = 100;

        if(i>=0 && i<15)
            vector2[i] = 100;
        else if(i>=15 && i<20)
            vector2[i] = 200;
        else if(i>=20 && i<25)
            vector2[i] = 40;
        else if(i>=25 && i<40)
            vector2[i] = 160;
        else
            vector2[i] = 100;
    }

    vcl_string vector1_filename = "/home/anilusumezbas/Desktop/appearance-DP-test/vector1.txt"; 
    vcl_string vector2_filename = "/home/anilusumezbas/Desktop/appearance-DP-test/vector2.txt";  

    vcl_ofstream vector1_file(vector1_filename.c_str());
    vcl_ofstream vector2_file(vector2_filename.c_str());

    for(unsigned n=0; n<50; ++n)
    {
        vector1_file << vector1[n] << vcl_endl;
        vector2_file << vector2[n] << vcl_endl;
    }

    vector1_file.close();
    vector2_file.close();

    dbcvr_cvmatch_intensity* DP = new dbcvr_cvmatch_intensity();
    DP->setPixelVector1(vector1);
    DP->setPixelVector2(vector2);
    DP->setTemplateSize(5);
    DP->Match();

    vcl_string corr1_filename = "/home/anilusumezbas/Desktop/appearance-DP-test/corr-1.txt";
    vcl_string corr2_filename = "/home/anilusumezbas/Desktop/appearance-DP-test/corr-2.txt";

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
