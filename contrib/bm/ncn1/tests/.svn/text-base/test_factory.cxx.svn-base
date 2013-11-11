//:
// \file

#include<testlib/testlib_test.h>

#include"../ncn1_factory.h"

#include<vul/vul_file.h>

void test_find_bin()
{
	//test find_bin
	ncn1_factory factory;
	vbl_array_1d<double> cdf;
	cdf.push_back(0.0); cdf.push_back(0.2); cdf.push_back(0.6); cdf.push_back(.97);
	
	TEST_EQUAL("Find Bin", factory.find_bin(cdf,0.1),1);
	TEST_EQUAL("Find Bin", factory.find_bin(cdf,-1),0);
	TEST_EQUAL("Find Bin", factory.find_bin(cdf,1),3);
	TEST_EQUAL("Find Bin", factory.find_bin(cdf,0.8),3);
	TEST_EQUAL("Find Bin", factory.find_bin(cdf,0.4),2);
}

void test_factory_binary_io()
{
    vcl_string cwd = vul_file::get_cwd();
    vcl_string binary_filename = cwd + "\\temporal_entropy.bin";
	vil_image_view<double> temporal_entropy;
    temporal_entropy.set_size(2,2,1);
    temporal_entropy(0,0,0) = 0.2; temporal_entropy(0,1,0) = 0.7;
    temporal_entropy(1,0,0) = 0.8; temporal_entropy(1,1,0) = 100;

    ncn1_factory factory;

    factory.set_temporal_entropy(temporal_entropy);
    factory.save_binary_temporal_entropy(binary_filename);
    
    ncn1_factory factory2;
    factory2.load_binary_temporal_entropy(binary_filename);
    vil_image_view<double> loaded_temporal_entropy = factory2.temporal_entropy();

    
    bool equal = true;

    if( temporal_entropy.size() == loaded_temporal_entropy.size() )
    {

        for( unsigned i = 0; i < loaded_temporal_entropy.ni(); ++i )
        {
            for( unsigned j = 0; j < loaded_temporal_entropy.nj(); ++j )
            {
                if(temporal_entropy(i,j,0) != loaded_temporal_entropy(i,j,0))
                    equal = false;
            }
        }
    }
    else
        equal = false;

    TEST_EQUAL("Testing factory binary io",equal,true);


}

static void test_factory()
{
    vcl_cout << '\n';
    vcl_cout << "Testing Find Bind function" << vcl_endl;
	test_find_bin();
    vcl_cout << "Testing Factory binary io" << vcl_endl;
    test_factory_binary_io();
}

TESTMAIN(test_factory);