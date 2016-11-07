//this is contrib/bm/ncn1/executables/temporal_entropy.cxx

#include"../ncn1_factory.h"
#include"../ncn1_utilities.h"

#include<vil/vil_convert.h>

#include<vil/vil_save.h>

int main(int argc, char* argv[])
{
    //user should provide video glob and results directory respectively

    if(argc < 2)
    {
        vcl_cerr << "error temporal_entropy.exe::must provide input glob and output directory" << vcl_endl;
        return 1;
    }

    vcl_string video_glob = argv[1];

    vcl_string result_dir = argv[2];

    vcl_cout << video_glob << vcl_endl;

    vcl_cout << result_dir << vcl_endl;

    vcl_string entropy_bin_filename = result_dir + "\\temporal_entropy.bin";

    vcl_string entropy_dat_filename = result_dir + "\\temporal_entropy.dat";

    ncn1_factory factory(video_glob,100,200,10);

    factory.calculate_temporal_entropy();

    factory.save_binary_temporal_entropy(entropy_bin_filename);

    factory.save_matlab_temporal_entropy(entropy_dat_filename);

    return 0;
}