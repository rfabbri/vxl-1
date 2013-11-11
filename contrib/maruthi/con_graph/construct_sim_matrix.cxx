// dbgl headers
//#include <dbskfg/pro/dbskfg_match_bag_of_curves_process.h>
 	
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vcl_fstream.h>
#include <vil/vil_transpose.h>
#include <vl/sift.h>

int main( int argc, char *argv[] )
{

    vcl_string file_name = 
        "/media/disk/ethz/Giraffes/texture2/image_pyramid/texture2_04.png";
    
    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(vil_load(file_name.c_str()));
    vil_image_view<vxl_byte> temp2=
        vil_transpose(temp);

    vil_image_view<double> image;
    vil_convert_cast(temp2,image);

    unsigned int width  = image.ni();
    unsigned int height = image.nj();

    vcl_cout<<"Width: "<<width<<vcl_endl;
    vcl_cout<<"Height: "<<height<<vcl_endl;

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* image_data=image.top_left_ptr();

    vcl_cout<<"Computing gradient"<<vcl_endl;

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        image_data,         // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride


    VlSiftFilt* filter = vl_sift_new(width,height,3,3,0);
    vl_sift_pix* sift_data=(vl_sift_pix*) 
        vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_magnitude[i];
        sift_data[index]=value;
        ++index;
    }

    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_angle[i];
        sift_data[index]=value;
        ++index;
    }
    
    vcl_cerr<<"Computing sift "<<vcl_endl;
    vl_sift_pix descr[128];

    vl_sift_calc_raw_descriptor(filter,
                                sift_data,
                                descr,
                                width,
                                height,
                                48.42,
                                49.76,
                                1,
                                -112*(M_PI/180));

    // write out sift point
    vcl_ofstream myfile("sift_point.txt");
    for ( unsigned int i =0 ; i < 128 ; ++i)
    {
        myfile<<descr[i]<<vcl_endl;
    }
    myfile.close();

    // void vl_sift_calc_raw_descriptor(filer
    // {
    //     vcl_ofstream myfile("grad_mag.txt");
    //     for ( unsigned int i=0; i < width*height ; ++i)
    //     {
    //         double value=gradient_magnitude[i];
    //         myfile<<value<<vcl_endl;
            
    //     }
    //     myfile.close();
    // }

    // {
    //     vcl_ofstream myfile2("grad_angle.txt");
    //     for ( unsigned int i=0; i < width*height ; ++i)
    //     {
    //         double value=gradient_angle[i];
    //         myfile2<<value<<vcl_endl;
            
    //     }
    //     myfile2.close();
    // }




    // vcl_stringstream stream1(argv[1]);
    // vcl_string model_cem_file;
    // stream1>>model_cem_file;

    // vcl_stringstream stream2(argv[2]);
    // vcl_string query_cem_file;
    // stream2>>query_cem_file;

    // vcl_stringstream stream3(argv[3]);
    // vcl_string output_prefix;
    // stream3>>output_prefix;



    // {
    //     // Call vidpro load cem process
    //     dbskfg_match_bag_of_curves_process match_pro;

    //     bpro1_filepath model_input(model_cem_file);
    //     bpro1_filepath query_input(query_cem_file);
    //     bpro1_filepath output_folder("/media/disk/contour_matches");

       
    //     match_pro.parameters()->set_value("-model_folder",model_input);
    //     match_pro.parameters()->set_value("-query_folder",query_input);
    //     match_pro.parameters()->set_value("-output_folder",output_folder);
    //     match_pro.parameters()->set_value("-output_prefix",output_prefix);

    //     // Before we start the process lets clean input output
    //     match_pro.clear_input();
    //     match_pro.clear_output();

    //     // Pass in input vsol string
    //     bool status = match_pro.execute();
    //     match_pro.finish();

    //     //Clean up after ourselves
    //     match_pro.clear_input();
    //     match_pro.clear_output();
    // }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);
    vl_sift_delete(filter);

    return 0;
}
