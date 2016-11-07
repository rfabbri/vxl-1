//:
// \file
// \program to extract extrema responses of images
// \author Brandon Mayer
// \date 14 May 09
//

#include"bmvr_functions.h"

#include<brip/brip_vil_float_ops.h> //fast extrema method

#include<bsta/bsta_histogram.h> //histogram class

#include<vcl_iostream.h> //cout etc...
#include<vcl_fstream.h> //write file output
#include<vcl_string.h> // std::string
#include<vcl_vector.h> // this is the vector container from stl
#include<vcl_map.h> //for the stl map container ( (key, value) pair)
#include<vcl_utility.h> // for vcl_pair::make_pair

#include<vil/vil_convert.h> //to convert images to and from vxl_byte and float
#include<vil/vil_image_view.h> //image views
#include<vil/vil_load.h> //load images
#include<vil/vil_math.h> //image math functions
#include<vil/vil_save.h> //save images


#include<vnl/vnl_matrix.h> //matricies
#include<vnl/vnl_vector.h> //vector to create the histogram

#include<vul/vul_file_iterator.h> //file iterator to parse files
#include<vul/vul_file.h> //strip extension from directory

using namespace std;

int main(int argc, char *argv[])
{
    vcl_string imageDir = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/NormalizedImages/",
               gtMaskDir = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/GroundTruthMasks/",
               opOutDir = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/OperatorOutput/",
               intHist = "/media/DATAPART1/BrandonDataFolder/VehicleRecognition/intensityHistograms/";

    
    // img.first = filename, img.second = vil_image_view pointer.
    vcl_map<vcl_string,vil_image_view<vxl_byte> > gtTemp;
    vcl_map<vcl_string,vil_image_view<bool> > gt;
    vcl_map<vcl_string,vil_image_view<float> > img, opOut, scaledImg;
    
    
    // load images
    vcl_string imgDir = imageDir + "*.png";
    for(vul_file_iterator fn = imgDir; fn; ++fn)
    {
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "Loading Image: " << filename << "\n";
        img.insert(vcl_make_pair(filename,vil_convert_cast(float(),vil_load(fn()))));
    }

    // load ground truths
    vcl_string gtDir = gtMaskDir + "*.png";
    for(vul_file_iterator fn = gtDir; fn; ++fn)
    {
        bool minb,maxb;
        vxl_byte min,max;
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "Loading Ground Truth: " << filename << "\n";
        gt.insert(vcl_make_pair(filename,vil_convert_cast(bool(),vil_load(fn()))));
    }

    //iterate through the loaded images
    for( vcl_map<vcl_string,vil_image_view<float> >::iterator it = img.begin();
         it != img.end(); ++it)
    {
        vcl_string currImg = it->first;
        unsigned ni = it->second.ni(),
                 nj = it->second.nj(),
                 nplanes = it->second.nplanes();

        float min=0.0f,max=0.0f;
        vcl_cout << "Processing " << currImg << vcl_endl;
        vil_math_value_range(img[currImg],min,max);
        vcl_cout << "Range = (" << min << ", " << max << ")" << vcl_endl;
        //create an empty image of appropriate size
        vil_image_view<float> temp(ni,nj,nplanes);
        opOut.insert(vcl_make_pair(currImg,temp));
        // note: YOU MUST SCALE THE IMAGE SO THAT IT'S RANGE IS BETWEEN 0 AND 1 TO APPLY
        // FAST EXTREMA!!!!
        scaledImg.insert(vcl_make_pair(currImg,temp));
        
        for(unsigned i = 0; i < ni; ++i)
            for(unsigned j = 0; j < nj; ++j)
                scaledImg[currImg](i,j,0) = img[currImg](i,j,0)/max;

        float smin=0.0f,smax=0.0f;
        vil_math_value_range(scaledImg[currImg],smin,smax);
        vcl_cout << "Range rescaled to: (" << smin << ", " << smax << ")" << vcl_endl;

        
        // apply extream operator to image
        vcl_cout << "Applying Extrema operator to image "<< currImg  << "..." <<vcl_endl;
        //point response always goes to plane 0
        bool bright = true; //just to remind myself i'm looking for either bright or dark cars
        bool output_response_mask = false; //if true mask will be copied to plane 1
        bool unclipped_response = false; //if no mask then goes to plane 1 if mask, then goes to plane 2
        //float cutoff_percentage = 0.0001f; //default is 0.01

        opOut[currImg] = brip_vil_float_ops::fast_extrema(scaledImg[currImg],2,1,-45,bright,output_response_mask,unclipped_response);

        //vcl_cout << "opOut[currImg].nplanes() = " << opOut[currImg].nplanes() << vcl_endl;

       // if the current image is an image for which we have a ground truth, calculate the 
       // distribution of operator response for that image
       if(currImg=="normalized0"||currImg=="normalized1"||currImg=="normalized3"||currImg=="normalized4"||
          currImg=="normalized6"||currImg=="normalized10"||currImg=="normalized16")
       {

           vcl_cout << "Creating ground truth histogram of operator response for image " << currImg << "..." <<"\n";
           vcl_string gtName = currImg+"_gt";
           vil_image_view<float>::iterator it = opOut[currImg].begin();
           vil_image_view<float>::iterator itEnd = opOut[currImg].end();
           unsigned nbins = 15; // arbitrarily set.
           float min,max,min_op,max_op;

           // get the range of the current operator response to set up histogram
           vil_math_value_range(opOut[currImg],min_op,max_op);
           min = 0.0; //we'll just start the histogram from 0 regardless of true min

           vcl_cout << "Min operator response = " << min_op << vcl_endl;
           vcl_cout << "Max operator response = " << max_op << vcl_endl;
               
           // make the histogram object
           bsta_histogram<float> hist(min,max_op,nbins);
           vcl_string dataName = opOutDir+currImg+".txt";
           vcl_ofstream dataStream(dataName.c_str(),vcl_ios_out);
           float thresh = 10e-3f;
           
           vcl_cout << "Writing operator response histogram data to " << currImg << ".txt " << vcl_endl;
           for(unsigned i = 0; i < ni; ++i)
              for(unsigned j = 0; j < nj; ++j)
                  if(gt[gtName](i,j)==1 && opOut[currImg](i,j,0) > thresh)
                  {
                      hist.upcount(opOut[currImg](i,j,0),1);
                      dataStream << opOut[currImg](i,j) << '\t';
                  }
                  
           // write the histogram to disk
           vcl_string outName = opOutDir+currImg+".m";
           vcl_ofstream ofstr(outName.c_str(),vcl_ios_out);
           hist.print_to_m(ofstr);



           //fit a weibull to the histogram...
           float k,lambda;
           if(bmvr_functions::fit_dist_to_response_hist(hist,k,lambda))
           {
               vcl_cout << "Weibull succesfully fit to operator histogram..." << vcl_endl;
           }
           else
           {
               vcl_cout << "Failed to fit Weibull to operator histogram..." << vcl_endl;
           }

           vcl_string outName2 = opOutDir+currImg+"_pdf.m";
           vcl_ofstream os(outName2.c_str(),vcl_ios_out);
           bmvr_functions::write_prob_dist_weibull_m(hist,os,lambda,k);
           os.close();
           
       }
       vcl_cout << "\n";
    }

 return 0;
}
