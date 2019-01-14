//this is contrib/bm/test_vidl.cxx

#include<string>
#include<ostream>

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_frame.h>

#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

int main()
{
    std::string img_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1\\*.jpg";

    vidl_image_list_istream video_list_stream(img_dir.c_str());

    std::cout << "num frames = " << video_list_stream.num_frames() << std::endl;

    unsigned frame_number = video_list_stream.frame_number();

    std::cout << "curr frame number = " << frame_number << std::endl;

    std::cout << "is seekable = " << video_list_stream.is_seekable() << std::endl;

    video_list_stream.advance();

    std::cout << "curr frame number after advance = " << video_list_stream.frame_number() << std::endl;;

    video_list_stream.seek_frame(10);

    std::cout << "curr fram after video_list_stream.seek_frame(10) = " << video_list_stream.frame_number() << std::endl;

    vil_image_view<vxl_byte> img;
    
    vidl_convert_to_view(*video_list_stream.current_frame(),img);

    vil_save(img,"C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\test_img.jpg");
    


    //for(unsigned i = 0; i < video_list_stream.num_frames();++i)
    //{
    //    video_list_stream.seek_frame(i);
    //}

    return 0;
}
