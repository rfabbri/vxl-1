#include "yong_image_icon_view.h"
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>

#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>



//-----------------------------------------------------------------------------
yong_image_icon_view::yong_image_icon_view(const char* n, const unsigned icon_width, const unsigned icon_height)
{
    image_name_.assign(n);
    this->w_ = icon_width;
    this->h_ = icon_height;

    vil_image_view<vxl_byte> original_image;


    vil_image_resource_sptr data = vil_load_image_resource(n);
    if (!data) return;
    original_image = data->get_view();
    unsigned original_w = data->ni();
    unsigned original_h = data->nj();

    this->nplanes_ = data->nplanes();

    float w_zooming_factor = (static_cast<float>(original_w))/(static_cast<float>(w_));
    float h_zooming_factor = (static_cast<float>(original_h))/(static_cast<float>(h_));

    vil_image_view<vxl_byte> temp_icon(icon_width, icon_height, data->nplanes());

    for(unsigned cur_plane=0; cur_plane<data->nplanes(); cur_plane++)
        for(unsigned cur_col=0; cur_col<w_; cur_col++)
            for(unsigned cur_row=0; cur_row<h_; cur_row++)
            {
                unsigned sample_i = static_cast<unsigned>(cur_col*w_zooming_factor);
                unsigned sample_j = static_cast<unsigned>(cur_row*h_zooming_factor);
                if(sample_i >= original_w)
                    sample_i = original_w-1;
                if(sample_j >= original_h)
                    sample_j = original_h-1;

                temp_icon(cur_col,cur_row,cur_plane) = original_image(sample_i, sample_j, cur_plane);
            }

    this->icon_image_.deep_copy(temp_icon);
    return;

}


//-----------------------------------------------------------------------------

vcl_string yong_image_icon_view::type_name() const
{
    return "yong_image_icon_view";
}


void yong_image_icon_view::b_write(vsl_b_ostream &os) const
{
    unsigned ver = 1;
    vsl_b_write(os, ver);
    vsl_b_write(os, icon_image_);
    vsl_b_write(os, w_);
    vsl_b_write(os, h_);
    vsl_b_write(os, nplanes_);
}

void yong_image_icon_view::b_read(vsl_b_istream &is)
{
    if (!is) return;

    unsigned ver;

    vsl_b_read(is, ver);

    switch(ver)
    {
    case 1:
        vsl_b_read(is, this->icon_image_);
        vsl_b_read(is, this->w_);
        vsl_b_read(is, this->h_);
        vsl_b_read(is, this->nplanes_);
        break;
    default:
        vcl_cout << "unsupported binary IO version for yong_image_icon_view!" << vcl_endl;
        break;
    }

    return;
}

void yong_image_icon_view::Output_Icon_Image()
{
    vcl_stringstream SS;
    vcl_string icon_image_name;

    SS << this->image_name_;
    SS << "_icon.jpg";

    SS >> icon_image_name;

    vil_save(this->icon_image_, icon_image_name.c_str());
}
