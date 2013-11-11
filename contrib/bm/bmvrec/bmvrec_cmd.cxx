//:
// \file
// \unsupervised learning of foreground vehicle distributions
// \author Brandon Mayer
// \date May 2009

#include "bmvrec_functions.h"

#include <vcl_iostream.h>
//#include <vcl_string.h>

int main()
{
    vcl_string imageDir = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/NormalizedImages/",
               gtDir = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/GroundTruthMasks/",
               outDir = "/home/brandon/research/vehicleRecognitionMatlab/";



    vcl_map<vcl_string,vil_image_view<bool> > gt;
    vcl_map<vcl_string,vil_image_view<float> > img, opOut;    

    //load the images and the ground truth
    img = bmvrec_functions::load_image_map_float(imageDir);
    gt = bmvrec_functions::load_image_map_bool(gtDir);

    vcl_string opResp = outDir+"operatorResponse.m";
    vcl_ofstream ofstr(opResp.c_str(),vcl_ios_out);

    vcl_string fg_opResp_underGt = outDir+"fg_opResp_underGt.m";
    vcl_ofstream fg_ofstr(fg_opResp_underGt.c_str(),vcl_ios_out);
    
    vcl_string weibull_params = outDir+"weibullParams.m";
    vcl_ofstream wbl_ofstr(weibull_params.c_str(),vcl_ios_out);

    

    //iterate through the images
    for(vcl_map<vcl_string,vil_image_view<float> >::iterator it=img.begin();
        it!=img.end(); ++it)
    {
        vcl_string currImg = it->first;

        unsigned ni = it->second.ni(), nj = it->second.nj(), nplanes = it->second.nplanes();
        float min = 0.0f, max = 0.0f;
        vcl_cout << '\n';
        vcl_cout << "Processing " << currImg << "..." << vcl_endl;
        vil_math_value_range(it->second,min,max);
        vcl_cout << "Intensity Range: (" << min << ", " << max << ")" <<  vcl_endl;

        //need to scale image to be within range 0 and 1 to apply fast extrema operator
        bmvrec_functions::scale_float(it->second,max);
        vil_math_value_range(it->second,min,max);
        vcl_cout << "Image Intensity Range Re-Scaled: (" << min << "," << max << ")" << vcl_endl;

        //apply operator to current image
        opOut[currImg] = bmvrec_functions::apply_extrema(it->second);
        float opMin = 0.0f, opMax = 0.0f;
        vil_math_value_range(opOut[currImg],opMin,opMax);
        vcl_cout << "Operator Response Range: (" << opMin << ", " << opMax << ")" << vcl_endl;

        //write operator response to disk
        bmvrec_functions::write_op_m(ofstr,opOut[currImg],currImg); 

        //if the image has a ground truth...
        if(bmvrec_functions::check_gt(currImg))
        {
            vcl_string gtName = currImg + "_gt";
            
            //extract the operator response under vehicles and write to disk
            bmvrec_functions::extract_fg_opRep(opOut[currImg],gt[gtName],currImg,fg_ofstr);

            
            
            
        }


        

    }

    ofstr.close();
    fg_ofstr.close();
    return 0;
}
