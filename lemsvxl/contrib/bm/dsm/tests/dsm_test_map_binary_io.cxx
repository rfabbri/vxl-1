//this is /contrib/bm/dsm/tests/dsm_test_map_map_binary_io.cxx
#include<dsm/io/dsm_io_map_map.h>
#include<dsm/io/dsm_io_map_map_map.h>

#include<testlib/testlib_test.h>

#include<vsl/vsl_binary_io.h>

template<class T1, class T2, class T3, class Compare1, class Compare2>
void test_map_map_binary_io()
{
    vcl_string filename("test_map_map_binary_io.bin");
    unsigned n = 5;

    {
        vcl_map<T1, vcl_map<T2,T3,Compare2>,Compare1> outer_map;

        for( unsigned i = 0; i < n; ++i )
        {
            vcl_map<T2,T3,Compare2> inner_map;
            for( unsigned j = 0; j < n; ++j )
            {
                inner_map[j] = n*i+j;
            }//end inner map iteration
            outer_map[i] = inner_map;
        }//end outer map iteration

        //write the file
        vsl_b_ofstream os(filename, vcl_ios::out|vcl_ios::binary);
        vsl_b_write(os,outer_map);
        os.close();
    }//temporary local scope

    //read from file
    vsl_b_ifstream is(filename, vcl_ios::in|vcl_ios::binary);
    vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1> outer_map2;
    vsl_b_read(is,outer_map2);
    is.close();
    
    for( unsigned i = 0; i < n; ++i )
        for( unsigned j = 0; j < n; ++j )
            TEST_NEAR("TEST READ MAP IS THE SAME AS WRITTEN",outer_map2[i][j], n*i+j,1);

}//end test_map_map_binary_io()

template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void test_map_map_map_binary_io()
{
    vcl_string filename("test_map_map_map_binary_io.bin");
    unsigned n = 5;

    {
        vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> outer_map;

        for( unsigned i = 0; i < n; ++i )
        {
            vcl_map<T2, vcl_map<T3, T4, Compare3>,Compare2> inner_map;
            for( unsigned j = 0; j < n; ++j )
            {
                vcl_map<T3, T4, Compare3> inner_map2;
                for( unsigned k = 0; k < n; ++k )
                {
                    inner_map2[k] = n*i*j+k;
                }//end inner map iteration 2
                inner_map[j] = inner_map2;
            }//end inner map iteration 1
            outer_map[i] = inner_map;
        }//end outer map iteration

        //write the file
        vsl_b_ofstream os(filename, vcl_ios::out|vcl_ios::binary);
        vsl_b_write(os,outer_map);
        os.close();
    }//temporary local scope

    //read from file
    vsl_b_ifstream is(filename, vcl_ios::in|vcl_ios::binary);
    vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> outer_map2;
    vsl_b_read(is, outer_map2);
    is.close();

    for( unsigned  i = 0; i < n; ++i )
        for( unsigned j = 0; j < n; ++j )
            for( unsigned k = 0; k < n; ++k )
                TEST_NEAR("TEST READ MAP IS THE SAME AS WRITTEN", outer_map2[i][j][k], n*i*j+k,1);

}//end test_map_map_map_binary_io()

static void dsm_test_map_binary_io()
{
    test_map_map_binary_io<unsigned,unsigned,unsigned,vcl_less<unsigned>,vcl_less<unsigned> >();
    test_map_map_binary_io<unsigned,unsigned,double,vcl_less<unsigned>,vcl_less<unsigned> >();
    test_map_map_map_binary_io<unsigned,unsigned,unsigned,unsigned,vcl_less<unsigned>,vcl_less<unsigned>,vcl_less<unsigned> >();
    test_map_map_map_binary_io<unsigned,unsigned,unsigned,double,vcl_less<unsigned>,vcl_less<unsigned>,vcl_less<unsigned> >();
    return;
}

TESTMAIN(dsm_test_map_binary_io);
