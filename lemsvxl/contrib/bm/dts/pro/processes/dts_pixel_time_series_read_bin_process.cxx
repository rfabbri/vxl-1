//this is /contrib/bm/dts/pro/processes/dts_pixel_time_series_read_bin_process.cxx
//:
// \file
// \date October 22, 2011
// \author Brandon A. Mayer
//
// Will create a new sptr from a binary file
//
// Inputs: 
//         1. filename		(std::string)
//         2. pixel type	(std::string)
//		   3. time type		(std::string)
//         4. math type		(std::string)
//		   5. dimension		(unsigned)
//
// Outputs:
//         None
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<dts/dts_pixel_time_series_base_sptr.h>
#include<dts/dts_pixel_time_series.h>

#include<vsl/vsl_binary_io.h>

namespace dts_pixel_time_series_read_bin_process_globals
{
    const unsigned int n_inputs_ = 5;
    const unsigned int n_outputs_ = 1;
}

//set input/outputs
bool dts_pixel_time_series_read_bin_process_cons( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_read_bin_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
	std::vector<std::string> output_types_(n_outputs_);

    unsigned i = 0;
    input_types_[i++] = vcl_string";//filename
	input_types_[i++] = vcl_string";//pixel type
	input_types_[i++] = vcl_string";//time type
	input_types_[i++] = vcl_string";//math type
	input_types_[i++] = "unsigned";//dimension

	output_types_[0] = "dts_pixel_time_series_base_sptr";

    if( !pro.set_input_types(input_types_))
        return false;

	if( !pro.set_output_types(output_types_))
		return false;
    
    return true;
}

bool dts_pixel_time_series_read_bin_process( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_read_bin_process_globals;

    if( pro.n_inputs() < n_inputs_ )
    {
        std::cout << pro.name()
                 << " dts_pixel_time_series_read_bin_process: "
                 << " The input number should be: "
                 << n_inputs_ << '\n'
                
                 << std::endl;
        return false;
    }

    //get inputs
    unsigned i = 0;
    std::string filename			= pro.get_input<std::string>(i++);
	std::string pixel_type		= pro.get_input<std::string>(i++);
	std::string time_type		= pro.get_input<std::string>(i++);
	std::string math_type		= pro.get_input<std::string>(i++);
	unsigned dim				= pro.get_input<unsigned>(i++);

    vsl_b_ifstream is(filename.c_str(), std::ios::in|std::ios::binary);

    if(!is)
        return false;

	dts_pixel_time_series_base_sptr ts_sptr;

	if( pixel_type == std::string("unsigned") &&
		time_type  == std::string("unsigned") &&
		math_type  == std::string("double") )
	{
		switch(dim)
		{
		case 1:
			{
				//create pointer on heap
				dts_pixel_time_series<unsigned,unsigned,double,1>* pts_ptr =
					new dts_pixel_time_series<unsigned,unsigned,double,1>();

				//read data from bin file
				pts_ptr->b_read(is);

				//make sptr to heap data
				ts_sptr = pts_ptr;

				break;
			}//end case 1
		case 2:
			{
				//create pointer on heap
				dts_pixel_time_series<unsigned,unsigned,double,2>* pts_ptr =
					new dts_pixel_time_series<unsigned,unsigned,double,2>();

				//read data from bin file
				pts_ptr->b_read(is);

				//make sptr to heap data
				ts_sptr = pts_ptr;

				break;
			}//end case 2
		case 3:
			{
				//create pointer on heap
				dts_pixel_time_series<unsigned,unsigned,double,3>* pts_ptr =
					new dts_pixel_time_series<unsigned,unsigned,double,3>();

				//read data from bin file
				pts_ptr->b_read(is);

				//make sptr to heap data
				ts_sptr = pts_ptr;

				break;
			}//end case 3
		case 10:
			{
				//create pointer on heap
				dts_pixel_time_series<unsigned,unsigned,double,10>* pts_ptr =
					new dts_pixel_time_series<unsigned,unsigned,double,10>();

				//read data from bin file
				pts_ptr->b_read(is);

				//make sptr to heap data
				ts_sptr = pts_ptr;

				break;
			}//end case 10
		case 128:
			{
				//create pointer on heap
				dts_pixel_time_series<unsigned,unsigned,double,128>* pts_ptr =
					new dts_pixel_time_series<unsigned,unsigned,double,128>();

				//read data from bin file
				pts_ptr->b_read(is);

				//make sptr to heap data
				ts_sptr = pts_ptr;

				break;
			}//end case 128
		default:
			{
				std::cerr << "----ERROR---- "
						 << "dts_pixel_time_series_read_bin_process: Unknown dimension, "
						 << " please augment.\n"
						 << "\tFILE: " << __FILE__ << '\n'
						 << "\tLINE: " << __LINE__ << '\n'
						 << std::flush;
				return false;
			}//end default
		}//end switch(dim)
	}//end if pixel_type=="unsigned" && time_type=="unsigned" && math_type=="double"
	else
	{
		std::cerr << "----ERROR---- "
				 << "dts_pixel_time_series_read_bin_process: pixel,"
				 << " time, math type combination. Please augment.\n"
				 << "\tFILE: " << __FILE__ << '\n'
				 << "\tLINE: " << __LINE__ << '\n'
				 << std::flush;
		return false;
	}

	pro.set_output_val(0,ts_sptr);

    return true;
}
