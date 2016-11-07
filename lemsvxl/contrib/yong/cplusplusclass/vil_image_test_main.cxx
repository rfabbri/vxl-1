#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <string.h>
#include <vcl_cstring.h>
#include <vcl_bitset.h>
#include <vcl_cassert.h>
#include <math.h>


#define GAMMA_ADJ(x, gamma) static_cast<vxl_byte>((pow((static_cast<double>(x))/256, gamma)*256))

int image_plane_split_demo()
{
    vil_image_view<vxl_byte> image;


    vil_image_resource_sptr data = vil_load_image_resource("color.jpg");
    if (!data) return 0;
    image = data->get_view();

    unsigned ni = data->ni();
    unsigned nj = data->nj();

    vil_image_view<vxl_byte> image_r(ni, nj, 1);
    vil_image_view<vxl_byte> image_g(ni, nj, 1);
    vil_image_view<vxl_byte> image_b(ni, nj, 1);

    for(unsigned j=0; j<nj; j++)
    for(unsigned i=0; i<ni; i++)
    {
        image_r(i,j,0) = image(i, j, 0);
        image_g(i,j,0) = image(i, j, 1);
        image_b(i,j,0) = image(i, j, 2);
    }

    vil_save(image_r, "color_r.jpg");
    vil_save(image_g, "color_g.jpg");
    vil_save(image_b, "color_b.jpg");

    return 1;
}


int image_gamma_adjustment_demo(double gamma, const char* output_filename)
{
    assert(gamma > 0);

    vil_image_view<vxl_byte> image;


    vil_image_resource_sptr data = vil_load_image_resource("wallpaper.jpg");
    if (!data) return 0;
    image = data->get_view();

    unsigned ni = data->ni();
    unsigned nj = data->nj();
    unsigned np = data->nplanes();

    vil_image_view<vxl_byte> output_image(ni, nj, 3);


//    double pixel_d; 

    for(unsigned p=0; p<np; p++)
    for(unsigned j=0; j<nj; j++)
    for(unsigned i=0; i<ni; i++)
    {
        output_image(i,j,p) = GAMMA_ADJ(image(i, j, p), gamma);

//        pixel_d = static_cast<double> (image(i, j, p));
//        pixel_d = pow(pixel_d/256, gamma) * 256;
//        output_image(i,j,p) = static_cast<vxl_byte>(pixel_d);
    }

    vil_save(output_image, output_filename);

    return 1;

}


int frame_croping_demo(vil_image_view<vxl_byte>& frame_buffer)
{
    static bool direction[3] = {0, 0, 0}; // 0 means forward, 1 means backward.
    static unsigned sampling_rate[3] = {2, 1, 3}; 
    static unsigned start_i[3] = {0, 0, 0};
    static unsigned len_i[3] = {720, 720, 720};
    static unsigned start_j[3] = {0, 450, 780};
    static unsigned len_j[3] = {450, 330, 500};
    static unsigned starting_frame[3] = {0, 980, 1500};
    static unsigned frame_len[3] = {80, 120, 70};
    static unsigned current_frame[3] = {0, 980, 1500};

    vcl_stringstream input_SS;
    vcl_string input_filename;
    vil_image_view<vxl_byte> image;

    for(unsigned crop_cnt=0; crop_cnt<3; crop_cnt++)
    {
        // compute the frame number of each crop
        if(!direction[crop_cnt]) // forward
        {
            current_frame[crop_cnt] += sampling_rate[crop_cnt];

            if(current_frame[crop_cnt] >= starting_frame[crop_cnt] + frame_len[crop_cnt])
            {
                direction[crop_cnt] = !direction[crop_cnt];
            }
        }
        else
        {
            current_frame[crop_cnt] -= sampling_rate[crop_cnt];

            if(current_frame[crop_cnt] <= starting_frame[crop_cnt])
            {
                direction[crop_cnt] = !direction[crop_cnt];
            }
        }


        input_SS.clear();
        input_filename.clear();
        input_SS << "input_sequence_" << current_frame[crop_cnt] << ".jpg";
        input_SS >> input_filename;
        vcl_cout << input_filename << vcl_endl;

        vil_image_resource_sptr data = vil_load_image_resource(input_filename.c_str());
        if (!data) return 0;
        image = data->get_view(start_i[crop_cnt], len_i[crop_cnt], start_j[crop_cnt], len_j[crop_cnt]);


        // copy the cropped view to the frame buffer;
        for(unsigned p=0; p<3; p++)
        for(unsigned j=0; j<len_j[crop_cnt]; j++)
        for(unsigned i=0; i<len_i[crop_cnt]; i++)
        {
            frame_buffer(i+start_i[crop_cnt],j+start_j[crop_cnt],p) = image(i,j,p);
        }
    }

    return 1;
}


#if 0

int main(int argc, char **argv)
{
    if(!image_plane_split_demo())
        return 0;

    double gamma = 0.3;
    if(!image_gamma_adjustment_demo(gamma, "wallpaper_gamma_0.3.jpg"))
        return 0;
    gamma = 0.8;
    if(!image_gamma_adjustment_demo(gamma, "wallpaper_gamma_0.8.jpg"))
        return 0;
    gamma = 1.2;
    if(!image_gamma_adjustment_demo(gamma, "wallpaper_gamma_1.2.jpg"))
        return 0;
    gamma = 3;
    if(!image_gamma_adjustment_demo(gamma, "wallpaper_gamma_3.jpg"))
        return 0;

    vcl_string output_image_sequence_name;
    vcl_stringstream output_name_SS;

    vil_image_view<vxl_byte> frame_buffer(1280, 720, 3);

    for(unsigned frame_cnt=0; frame_cnt<300; frame_cnt++)
    {
        frame_croping_demo(frame_buffer);

        output_name_SS.clear();
        output_name_SS << "output_" << frame_cnt << ".jpg";
        output_name_SS >> output_image_sequence_name;
        vil_save(frame_buffer, output_image_sequence_name.c_str());
    }

  return 1;
}

#endif

