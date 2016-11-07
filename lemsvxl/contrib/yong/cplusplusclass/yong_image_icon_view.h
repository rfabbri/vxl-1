#ifndef yong_image_icon_view_h_
#define yong_image_icon_view_h_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vil/vil_image_view.h>
#include <cplusplusclass/yong_image_icon_view_sptr.h>
#include <vxl_config.h> // for vxl_byte
#include <vbl/vbl_ref_count.h>
#include <vil/io/vil_io_image_view.h>


struct yong_image_icon_view_new;


class yong_image_icon_view : public vbl_ref_count 
{

    public:
    
    unsigned w_;
    unsigned h_;
    unsigned nplanes_;
    vil_image_view<vxl_byte> icon_image_;

    //: Constructor - don't use this, use yong_image_icon_view_new.
    yong_image_icon_view(const char* n="unnamed", const unsigned icon_width=100, const unsigned icon_height=80);


    ~yong_image_icon_view() { }
    vcl_string yong_image_icon_view::type_name() const;
    

    //: Name of this tableau.
    vcl_string image_name_;

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    void Output_Icon_Image();

};


//-------------------------------------------------------------
// Make a smart-pointer constructor for our view.
struct yong_image_icon_view_new : public yong_image_icon_view_sptr
{
  //: Constructor - create an easy2D with the given name.
  yong_image_icon_view_new(char const *n="unnamed", const unsigned icon_width=100, const unsigned icon_height=80) :
    yong_image_icon_view_sptr(new yong_image_icon_view(n, icon_width, icon_height)) { }

};

#endif // 
