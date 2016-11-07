//:
// \file
// \Functions for Vehicle Recognition
// \author Brandon Mayer
// \date 24 May 09
#ifndef bmvr_functions_h_
#define bmvr_functions_h_

#include<bsta/bsta_histogram.h>
#include<bsta/bsta_weibull.h>
#include<bsta/algo/bsta_fit_weibull.h>

#include<vcl_iostream.h>
#include<vcl_fstream.h>

#include<vil/vil_image_view.h>

class bmvr_functions
{
public:
    ~bmvr_functions(){}
// to quickly iterate through an image and output to error stream if
// there is a number other than 0 or 1. This is to check if loading a
// binary image worked.
    static void checkBinary(vcl_ostream &os, vil_image_view<vxl_byte>& img);

    static void convertByte_Bool(const vil_image_view<vxl_byte>& in, vil_image_view<bool>& out);

    static void convertBool_Byte(const vil_image_view<bool>& in, vil_image_view<vxl_byte>& out);

    static bool fit_dist_to_response_hist(bsta_histogram<float>& hist, float& k, float& lambda);

    // this function is necissary because bsta_histogram::p return the probability of a given bin,
    // that is (number of counts in a specific bin)/(total number of counts). To view an approximation
    // of the operator response probability density, we must do: 
    // (number of counts in a specific bin)/{(total number of counts)*binWidth}
    // in this way we may compare our observations with the weibull pdf. Otherwise, this comparison
    // is invalid.
    static void write_prob_dist_to_m(bsta_histogram<float>& hist, vcl_ofstream& os);

    static void write_prob_dist_weibull_m(bsta_histogram<float>& hist,vcl_ofstream& os,float& scale, float& shape); 

    //this will integrate the fitted weibull distribution to the bin width, therefore describing bin probability via weibull
    //instead of describing a weibull pdf.
    /* static void write_prob_bin_weibull_m(bsta_histogram<float>& hist,vcl_ofstream& os, float& scale, float& shape, float& minW, float& maxW); */
        
};
#endif //bmvr_functions_h_
