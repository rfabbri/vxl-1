// This is brcv/mvl/dbrct_wide_dense_match.h

#ifndef _dbrct_wide_dense_match_h_
#define _dbrct_wide_dense_match_h_
//:
// \file
// \brief This class is an interface to do dense matching on images obtained from wide-baseline stereo. 
//        It includes a coarse to fine strategy for dense matching.
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 08/31/04

#include <vcl_iostream.h>
#include <vil1/vil1_image.h>
#include <vnl/vnl_matrix.h>
#include <dbrct/dbrct_wide_dense_match_params.h>

class dbrct_wide_dense_match
{

 public:
    dbrct_wide_dense_match();
    dbrct_wide_dense_match(vil1_image imgl,vil1_image imgr);
        dbrct_wide_dense_match(vil1_image imgl,vil1_image imgr, int levels,int samplingrate);
    ~dbrct_wide_dense_match();

    //: to scale pair of images
    bool scale_images();

        //: build a pyramid of images
        bool downsample_images();

  
        bool do_dense_matching();

    //: get the correspondence
    vnl_matrix<int> get_correspondence();

    //: interpolate the correspondence up to the original scale
    bool interp_correspondence(vnl_matrix<int> iassignment,vnl_matrix<int> & fassignment);

    //: scale down or scale up the image
    bool scale_img(vil1_image img_in,int scale,vil1_image & img_out);

        
    //: output the matrices 
    void display_matrices(vnl_matrix<int> p);


    vil1_image imgl_;
    vil1_image imgr_;


        vcl_vector<vil1_image> scaledimgsl;
        vcl_vector<vil1_image> scaledimgsr;

    dbrct_wide_dense_match_params params;

vnl_matrix<int> f_assignments;
private:
        //: original image size
    int orig_w_;
    int orig_h_;

    


};



#endif
