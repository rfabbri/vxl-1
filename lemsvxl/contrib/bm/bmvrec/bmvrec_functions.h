//:
// \file
// \functions for learning of foreground bmvr_cmd.cxx
// \author Brandon Mayer
// \date May 2009

#ifndef bmvrec_functions_h_
#define bmvrec_functions_h_

#include<brip/brip_vil_float_ops.h> //fast extrema method

#include<bsta/bsta_histogram.h> //histogram class
//#include "/home/brandon/vxl/contrib/brl/bbas/bsta/bsta_histogram.h" //histogram class
//#include<bsta/bsta_gauss_id3.h>

#include<vcl_fstream.h> //file io
#include<vcl_map.h> //for the stl map container, (key,value) pair
#include<vcl_string.h> // std:string class
#include<vcl_utility.h> //for vcl_pair::make_pair


#include<vil/vil_convert.h> //to convert image types
#include<vil/vil_load.h> //to load images
#include<vil/vil_image_view.h> // class to store/manipulate images

#include<vul/vul_file_iterator.h> // file iterator to parse directory 
#include<vul/vul_file.h> //function to strip extension from directory

class bmvrec_functions
{
public:
    bmvrec_functions(){}
    ~bmvrec_functions(){}

    //loading the images
    //parse given directory for *.png files
    static vcl_map<vcl_string,vil_image_view<float> > load_image_map_float(const vcl_string& fullImgDirectory);

    //load the ground truths
    //parse given directory for *.png files
    static vcl_map<vcl_string,vil_image_view<bool> > load_image_map_bool(const vcl_string& fullImgDirectory);

    //scale float image to be within range 0 and 1
    static void scale_float(vil_image_view<float>& img, const float& max);
    
    //apply extrema operator to image
    static vil_image_view<float> apply_extrema(vil_image_view<float>& img);

    //write operator response to disk
    static void write_op_m(vcl_ofstream& os, const vil_image_view<float>& opRep,const vcl_string& imgName);
    
    //check if the current image has a ground truth mask
    static bool check_gt(const vcl_string& currImg);

    //extract foreground operator response histogram and write data to disk to disk
    static void extract_fg_opRep(const vil_image_view<float>& opRep, const vil_image_view<bool>& gt,const vcl_string& currImg,
                                 vcl_ofstream& os);
    
    
};

#endif //bmvrec_functions_h_
