#ifndef io_yong_image_icon_view_h_
#define io_yong_image_icon_view_h_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vil/vil_image_view.h>
#include <cplusplusclass/yong_image_icon_view_sptr.h>
#include <vxl_config.h> // for vxl_byte
#include <vbl/vbl_ref_count.h>
#include <vil/io/vil_io_image_view.h>
#include <vcl_cstddef.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <cplusplusclass/yong_image_icon_view.h>


inline void vsl_b_write(vsl_b_ostream &os, yong_image_icon_view const icon)
{

    unsigned ver = 1;
    vsl_b_write(os, ver);
    vsl_b_write(os, icon.icon_image_);
    vsl_b_write(os, icon.w_);
    vsl_b_write(os, icon.h_);
    vsl_b_write(os, icon.nplanes_);

}


inline void vsl_b_read(vsl_b_istream &is, yong_image_icon_view& icon)
{
    if (!is) return;

    unsigned ver;

    vsl_b_read(is, ver);

    switch(ver)
    {
    case 1:
        vsl_b_read(is, icon.icon_image_);
        vsl_b_read(is, icon.w_);
        vsl_b_read(is, icon.h_);
        vsl_b_read(is, icon.nplanes_);
        break;
    default:
        vcl_cout << "unsupported binary IO version for yong_image_icon_view!" << vcl_endl;
        break;
    }

    return;
}

inline void vsl_b_write(vsl_b_ostream &os, yong_image_icon_view_sptr const icon)
{

    unsigned ver = 1;
    vsl_b_write(os, ver);
    vsl_b_write(os, icon->icon_image_);
    vsl_b_write(os, icon->w_);
    vsl_b_write(os, icon->h_);
    vsl_b_write(os, icon->nplanes_);

}


inline void vsl_b_read(vsl_b_istream &is, yong_image_icon_view_sptr& icon)
{
    if (!is) return;

    unsigned ver;

    vsl_b_read(is, ver);

    switch(ver)
    {
    case 1:
        vsl_b_read(is, icon->icon_image_);
        vsl_b_read(is, icon->w_);
        vsl_b_read(is, icon->h_);
        vsl_b_read(is, icon->nplanes_);
        break;
    default:
        vcl_cout << "unsupported binary IO version for yong_image_icon_view!" << vcl_endl;
        break;
    }

    return;
}

inline void vsl_print_summary(vcl_ostream& os, yong_image_icon_view_sptr const icon)
{
   return;
}

inline void vsl_print_summary(vcl_ostream& os, yong_image_icon_view const icon)
{
   return;
}

#endif // 

