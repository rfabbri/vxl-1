

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vcl_sstream.h>
#include <vl/mathop.h>
#include <vl/imopv.h>
#include <vl/sift.h>
#include <vnl/vnl_vector.h>
#include <vil/algo/vil_orientations.h>

void compute_grad_maps(vil_image_resource_sptr& input_image,
                       vl_sift_pix** grad_data,
                       VlSiftFilt** filter);

void compute_grad_color_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    unsigned int channel);

void compute_sobel_grad_color_maps(
    vil_image_resource_sptr& input_image,
    double** grad_data,
    unsigned int channel);

void compute_grad_maps(vil_image_resource_sptr& input_image,
                       vl_sift_pix** grad_data,
                       VlSiftFilt** filter)
{

    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(
            0.2989,
            0.5870,
            0.1140,
            input_image->get_view());
    
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

    *filter   = vl_sift_new(width,height,-1,-1,0);
    *grad_data= (vl_sift_pix*) 
        vl_malloc(sizeof(vl_sift_pix)*width*height*2);
   
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];

        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}


void compute_grad_color_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    unsigned int channel)
{

    vil_image_view<vxl_byte> imview = input_image->get_view();
    vil_image_view<vxl_byte> temp   = vil_plane(imview,channel);
    
    vil_image_view<double> image;
    vil_convert_cast(temp,image);

    unsigned int width  = image.ni();
    unsigned int height = image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* image_data=image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        image_data,         // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride

    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];
        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);




}

void compute_sobel_grad_color_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    unsigned int channel)
{

 
    vil_image_view<vxl_byte> imview = input_image->get_view();
    vil_image_view<vxl_byte> temp   = vil_plane(imview,channel);

    unsigned int width  = temp.ni();
    unsigned int height = temp.nj();

    vil_image_view<vl_sift_pix> grad_mag;
    vil_image_view<vl_sift_pix> grad_angle;
    
    vil_orientations_from_sobel(temp,grad_mag,grad_angle);

    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    vl_sift_pix* gradient_magnitude=grad_mag.top_left_ptr();
    vl_sift_pix* gradient_angle=grad_angle.top_left_ptr();

    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];
        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
        ++index;
    }



}

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

    vcl_stringstream color_stream(argv[6]);
    double color(0.0);
    color_stream>>color;

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(input_img.c_str());

    if ( !color )
    {
    
        vl_sift_pix* grad_data(0);
        VlSiftFilt* filter(0);
        
        compute_grad_maps(img_sptr,
                          &grad_data,
                          &filter);

        vl_sift_set_magnif(filter,1.0);
        
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
        
        vl_sift_delete(filter);
        vl_free(grad_data);


    }
    else
    {

        vcl_cout<<"Computing color"<<vcl_endl;

        unsigned int width  = img_sptr->ni();
        unsigned int height = img_sptr->nj();

        vl_sift_pix* red_grad_data(0);
        vl_sift_pix* green_grad_data(0);
        vl_sift_pix* blue_grad_data(0);

        VlSiftFilt* filter = vl_sift_new(width,height,-1,-1,0);
        
        compute_grad_color_maps(img_sptr,
                                &red_grad_data,
                                0);

        compute_grad_color_maps(img_sptr,
                                &green_grad_data,
                                1);

        compute_grad_color_maps(img_sptr,
                                &blue_grad_data,
                                2);

        vl_sift_set_magnif(filter,1.0);

        vl_sift_pix descr_ps1_red[128];
        memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
        vl_sift_pix descr_ps1_green[128];
        memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
        vl_sift_pix descr_ps1_blue[128];
        memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);


        vl_sift_calc_raw_descriptor(filter,
                                    red_grad_data,
                                    descr_ps1_red,
                                    filter->width,
                                    filter->height,
                                    x_coord, 
                                    y_coord,
                                    scale,
                                    theta);

        vl_sift_calc_raw_descriptor(filter,
                                    green_grad_data,
                                    descr_ps1_green,
                                    filter->width,
                                    filter->height,
                                    x_coord, 
                                    y_coord,
                                    scale,
                                    theta);

        vl_sift_calc_raw_descriptor(filter,
                                    blue_grad_data,
                                    descr_ps1_blue,
                                    filter->width,
                                    filter->height,
                                    x_coord, 
                                    y_coord,
                                    scale,
                                    theta);


        
        vnl_vector<vl_sift_pix> descr1(384,0.0);

        for ( unsigned int d=0; d < 128 ; ++d)
        {
            descr1.put(d,descr_ps1_red[d]);
            descr1.put(d+128,descr_ps1_green[d]);
            descr1.put(d+256,descr_ps1_blue[d]);
        }

        
        for ( unsigned int i=0; i < 384; ++i)
        {
            vcl_cout<<512*descr1[i]<<vcl_endl;
            
        }
        
        vl_sift_delete(filter);
        vl_free(red_grad_data);
        vl_free(green_grad_data);
        vl_free(blue_grad_data);


    }
    return 0;
}
