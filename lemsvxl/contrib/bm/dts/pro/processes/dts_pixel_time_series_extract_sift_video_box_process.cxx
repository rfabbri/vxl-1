//this is /contrib/bm/dts/pro/processes/dts_extract_sift_video_box_process.cxx
//:
// \file
// \date October 7, 2011
// \author Brandon A. Mayer
//
// Process to extract sift points from a fixed box
// withing a video sequence
// NOTE: To extract sift at every pixel, specify
//       xmin = xmax = ymin = ymax = 0
//
// Inputs: 
//         1. video glob (vcl_string)
//         2. time type (vcl_string)
//         2. xmin (unsigned)
//         3. xmax (unsigned)
//         4. ymin (unsigned)
//         5. ymax (unsigned)
// Outputs:
//         1. dts_pixel_time_series_base_sptr 
//
// \verbatim
//  Modifications
// \endverbatim
#include<bapl/bapl_dsift_sptr.h>

#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<dts/dts_time_series_base_sptr.h>
#include<dts/dts_time_series.h>

#include<dts/dts_pixel_time_series.h>
#include<dts/dts_pixel_time_series_base_sptr.h>

#include<vcl_vector.h>

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_resource_sptr.h>
#include<vil/vil_image_view_base.h>

namespace dts_pixel_time_series_extract_sift_video_box_process_globals
{
    const unsigned int n_inputs_ = 6;
    const unsigned int n_outputs_ = 1;
}

//set input/output types
bool dts_pixel_time_series_extract_sift_video_box_process_cons( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_extract_sift_video_box_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    vcl_vector<vcl_string> output_types_(n_outputs_);

    unsigned i = 0;
    input_types_[i++] = "vcl_string";//video glob
    input_types_[i++] = "vcl_string";//pixel type
    input_types_[i++] = "unsigned";//xmin
    input_types_[i++] = "unsigned";//xmax
    input_types_[i++] = "unsigned";//ymin
    input_types_[i++] = "unsigned";//ymax

    output_types_[0] = "dts_pixel_time_series_base_sptr";

    if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;

}//end process cons


bool dts_pixel_time_series_extract_sift_video_box_process( bprb_func_process& pro )
{
    using namespace 
        dts_pixel_time_series_extract_sift_video_box_process_globals;

    if( pro.n_inputs() < n_inputs_ )
    {
        vcl_cout << pro.name() 
            << " dts_extract_sift_video_box_proces: "
            << " The input number should be " 
            << n_inputs_ 
            << vcl_endl;
        return false;
    }

    //get inputs
    unsigned i = 0;
    vcl_string video_glob = pro.get_input<vcl_string>(i++);
    vcl_string pixel_type = pro.get_input<vcl_string>(i++);
    unsigned xmin = pro.get_input<unsigned>(i++);
    unsigned xmax = pro.get_input<unsigned>(i++);
    unsigned ymin = pro.get_input<unsigned>(i++);
    unsigned ymax = pro.get_input<unsigned>(i++);

    vidl_image_list_istream video_stream(video_glob);

    unsigned nframes		= video_stream.num_frames();
	unsigned frameWidth		= video_stream.width();
	unsigned frameHeight    = video_stream.height();

    if( nframes == 0 )
    {
        vcl_cerr << "---- Error ---- "
            << "dts_extract_sift_video_box_proces:\n"
            << "\tThe video stream has no frames.\n" 
            << vcl_flush;
        return false;
    }

    if( pixel_type == "unsigned" )
    {
        dts_pixel_time_series_base_sptr dts_sptr = 
            new dts_pixel_time_series<unsigned, unsigned, double, 128>();

		dts_pixel_time_series<unsigned,unsigned,double,128>* dts_ptr =
			static_cast<dts_pixel_time_series<unsigned, unsigned, double, 128>*>
			(dts_sptr.as_pointer());

        for( unsigned frame = 0; frame < nframes; ++frame )
        {
//#ifdef _DEBUG
            vcl_cout << "Extracting SIFT frame: " << frame
                     << " of " << nframes << vcl_endl;
//#endif //_DEBUG

            vil_image_view<vxl_byte> curr_img;
            video_stream.seek_frame(frame);
            vidl_convert_to_view(
                *video_stream.current_frame(), curr_img);

            bapl_dsift dsift;

            if( curr_img.nplanes() != 1 || 
                curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
            {
                vil_image_view<vxl_byte> grey_img;
                vil_convert_planes_to_grey(curr_img, grey_img);
                dsift.set_img(grey_img);
            }
            else
            {
                dsift.set_img(curr_img);
            }

			if( xmin == 0 &&
				xmax == 0 &&
				ymin == 0 &&
				ymax == 0)
			{
#ifdef _DEBUG
				vcl_cout << "Extracting SIFT at Every Pixel in the Frame "
					     << "(" << frameWidth*frameHeight << "pixels)." << vcl_endl;
#endif //_DEBUG

				for( unsigned x = 0; x < frameWidth; ++x )
					for( unsigned y = 0; y < frameHeight; ++y )
					{
						vnl_vector_fixed<double,128> feature(dsift.vnl_dsift(x,y));
						dts_ptr->insert(vgl_point_2d<unsigned>(x,y),frame,feature);
					}
			}//end extract the whole frame
			else
			{
#ifdef _DEBUG
				vcl_cout << "Extracting SIFT in box: "
					     << "(xmin = " << xmin << ","
						 << "xmax = " << xmax << ","
						 << "ymin = " << ymin << ","
						 << "ymax = " << ymax << ")" << vcl_endl;
#endif //_DEBUG

				for( unsigned x = xmin; x <= xmax; ++x )
					for( unsigned y = ymin; y <= ymax; ++y )
					{
#if 0
						vcl_cout << "\tExtracting SIFT at pixel: (" 
							<< x << ',' << y << ")" << vcl_endl;
#endif //_DEBUG
						vnl_vector_fixed<double,128> feature(dsift.vnl_dsift(x,y));
						dts_ptr->insert(vgl_point_2d<unsigned>(x,y),frame,feature);
					}//end pixel iteration
			}//end else xmin = xmax = ymin = ymax = 0
		}//end frame iteration



		//dts_pixel_time_series_base_sptr pts_sptr(pts_ptr);
        
        pro.set_output_val(0,dts_sptr);
        return true;
    }
    else
    {
        vcl_cerr << "----ERROR----\n"
            << "\tdts_extract_sift_video_box_process: \n"
            << "\t\t Unknown pixel type, please augment.\n"
            << vcl_flush;
        return false;
    }

    return false;
}//end process