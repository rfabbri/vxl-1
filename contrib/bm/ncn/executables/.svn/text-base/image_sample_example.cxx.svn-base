//this is /contrib/bm/ncn/executables/image_sample_example.cxx

#include<vgl/vgl_point_2d.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_view.h>
#include<vil/vil_resample_nearest.h>
#include<vil/vil_resample_bicub.h>
#include<vil/vil_save.h>

int main()
{
	vcl_string img_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1\\*.jpg";
	vcl_string out_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\";

	vidl_image_list_istream video_list(img_dir.c_str());

	video_list.seek_frame(0);

	vil_image_view<vxl_byte> img,grey,sample;

	vidl_convert_to_view(*video_list.current_frame(),img);
	vil_convert_planes_to_grey<vxl_byte,vxl_byte>(img,grey);
	vil_save(grey,(out_dir + "test_image2.jpg").c_str());

	vil_resample_bicub<vxl_byte,vxl_byte>(grey,sample,500,500);

	vil_save(sample,(out_dir + "sample_test.jpg").c_str());

	





	return 0;
}