

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vcl_sstream.h>
#include <vl/mathop.h>
#include <vl/imopv.h>
#include <vl/sift.h>



int main( int argc, char *argv[] )
{

    vcl_stringstream stream(argv[1]);
    vcl_string input_img;
    stream>>input_img;


    vcl_stringstream x_stream(argv[2]);
    double x_coord(0.0);
    x_stream>>x_coord;

    vcl_stringstream y_stream(argv[3]);
    double y_coord(0.0);
    y_stream>>y_coord;


    vcl_stringstream theta_stream(argv[4]);
    double theta(0.0);
    theta_stream>>theta;
    theta=theta*(VL_PI/180);
    vcl_cout<<theta<<vcl_endl;

    vcl_stringstream scale_stream(argv[5]);
    double scale(0.0);
    scale_stream>>scale;

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(input_img.c_str());

    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(
            0.2989,
            0.5870,
            0.1140,
            img_sptr->get_view());
    
    vil_image_view<double> orig_image;
    vil_convert_cast(temp,orig_image);

    unsigned int width  = orig_image.ni();
    unsigned int height = orig_image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* orig_image_data=orig_image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        orig_image_data,    // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride

    VlSiftFilt* filter = vl_sift_new(width,height,-1,-1,0);
    vl_sift_pix* grad_data=(vl_sift_pix*) 
        vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    vl_sift_set_magnif(filter,1.0);

    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];

        (grad_data)[index]=mag;
        ++index;
        (grad_data)[index]=angle;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);


    vl_sift_pix descr_ps1[128];
    memset(descr_ps1, 0, sizeof(vl_sift_pix)*128);


    vl_sift_calc_raw_descriptor(filter,
                                grad_data,
                                descr_ps1,
                                filter->width,
                                filter->height,
                                x_coord, 
                                y_coord,
                                scale,
                                theta);



    
    for ( unsigned int i=0; i < 128; ++i)
    {
        vcl_cout<<512*descr_ps1[i]<<vcl_endl;

    }
    return 0;
}
