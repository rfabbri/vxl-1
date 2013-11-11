//this is contrib/brl/bm/ncn1/executables/westin_full.cxx

#include"../ncn1_factory.h"
#include"../ncn1_utilities.h"

#include<vil/vil_convert>

#include<vil/vil_save.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        vcl_cerr << "error westin_full.exe::must provide input glob and output directory" << vcl_endl;
        return 1;
    }

    vcl_string video_glob = argv[1];

    vcl_string result_dir = argv[2];

    vcl_string entropy_bin_filename = result_dir + "\\temporal_entropy.bin";

    ncn1_factory factory;

    factory.load_binary_temporal_entropy(entropy_bin_filename);



}