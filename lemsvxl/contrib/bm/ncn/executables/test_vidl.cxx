//this is contrib/bm/test_vidl.cxx

#include<vcl_string.h>
#include<vcl_ostream.h>

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_frame.h>

#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

int main()
{
    vcl_string img_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1\\*.jpg";

    vidl_image_list_istream video_list_stream(img_dir.c_str());

    vcl_cout << "num frames = " << video_list_stream.num_frames() << vcl_endl;

    unsigned frame_number = video_list_stream.frame_number();

    vcl_cout << "curr frame number = " << frame_number << vcl_endl;

    vcl_cout << "is seekable = " << video_list_stream.is_seekable() << vcl_endl;

    video_list_stream.advance();

    vcl_cout << "curr frame number after advance = " << video_list_stream.frame_number() << vcl_endl;;

    video_list_stream.seek_frame(10);

    vcl_cout << "curr fram after video_list_stream.seek_frame(10) = " << video_list_stream.frame_number() << vcl_endl;

    vil_image_view<vxl_byte> img;
    
    vidl_convert_to_view(*video_list_stream.current_frame(),img);

    vil_save(img,"C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\test_img.jpg");
    


    //for(unsigned i = 0; i < video_list_stream.num_frames();++i)
    //{
    //    video_list_stream.seek_frame(i);
    //}

    return 0;
}
