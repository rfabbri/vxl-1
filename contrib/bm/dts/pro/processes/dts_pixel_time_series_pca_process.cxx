//this is /contrib/bm/dts/processes/dts_pixel_time_series_pca.cxx
//:
// \file
// \date November 4, 2011
// \author Brandon A. Mayer
//
// Process to reduce dimensionality of features in dts_pixel_time_series_base_sptr
// Process will output a new smart pointer with reduced dimensionality
//
// Inputs: 1. dts_pixel_time_series_base_sptr
//		   2. pixel type (string)
//         3. time type (string)
//         4. math type (string)
//		   5. original dimension (unsigned)
//         6. reduced dimension (unsigned)
//
// Outputs: 1. dts_pixel_time_series_base_sptr
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dts/dts_pixel_time_series_base_sptr.h>
#include<dts/dts_pixel_time_series.h>

#include<dts/algo/dts_pixel_time_series_pca_vnl_vector_fixed.h>


//: global variables
namespace dts_pixel_time_series_pca_process_globals
{
    const unsigned int n_inputs_ = 6;
    const unsigned int n_outputs_ = 1;
}

//set input/output types
bool dts_pixel_time_series_pca_process_cons( bprb_func_process& pro )
{
	using namespace dts_pixel_time_series_pca_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

    unsigned i = 0;

	input_types_[i++] = "dts_pixel_time_series_base_sptr";
	input_types_[i++] = "vcl_string";//pixel_type
	input_types_[i++] = "vcl_string";//time_type
    input_types_[i++] = "vcl_string";//math_type
    input_types_[i++] = "unsigned";//original dimension
    input_types_[i++] = "unsigned";//reduced dimension

	output_types_[0] = "dts_pixel_time_series_base_sptr";

	if(!pro.set_input_types(input_types_))
        return false;

	if(!pro.set_output_types(output_types_))
		return false;

    return true;
}//end dts_pixel_time_series_pca_process_cons

bool dts_pixel_time_series_pca_process( bprb_func_process& pro )
{
	using namespace dts_pixel_time_series_pca_process_globals;

    if( pro.n_inputs() < n_inputs_ )
    {
        vcl_cerr << "----ERROR---- " 
                 << pro.name() << '\n'
                 << "\tdts_pixel_time_series_pca_process: The input number should be: "
                 << n_inputs_ << '\n'
                 << "\tFILE: " << __FILE__ << '\n'
                 << "\tLINE: " << __LINE__ << '\n'
                 << vcl_flush;
    }

	//get inputs
    unsigned i = 0;
    dts_pixel_time_series_base_sptr pts_sptr = 
        pro.get_input<dts_pixel_time_series_base_sptr>(i++);

	vcl_string pixel_type = 
		pro.get_input<vcl_string>(i++);

    vcl_string time_type =
        pro.get_input<vcl_string>(i++);

    vcl_string element_type = 
        pro.get_input<vcl_string>(i++);

    unsigned src_dim = 
        pro.get_input<unsigned>(i++);

    unsigned dest_dim =
        pro.get_input<unsigned>(i++);

	dts_pixel_time_series_base_sptr pts_output_sptr;

	if( pixel_type == "unsigned" && 
		time_type == "unsigned" && 
		element_type == "double" )
	{
		switch(src_dim)
		{
		case 128:
			{
				//cast to correct child
				dts_pixel_time_series<unsigned,unsigned,double,128>* pts_ptr =
					dynamic_cast<dts_pixel_time_series<unsigned,unsigned,double,128>*>(pts_sptr.as_pointer());

				//check successful cast
				if( !pts_ptr )
				{
					vcl_cerr << "----ERROR---- "
                         << "dts_pca_process: Could not cast from base to child."
						 << "\tpixel_type = " << pixel_type << '\n'
						 << "\ttime_type = " << time_type << '\n'
						 << "\telement_type = " << element_type << '\n'
						 << "\tsrc_dim = " << src_dim << '\n'
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                         << vcl_flush;
					return false;
				}// end null check

				switch(dest_dim)
				{
				case 2:
					{
						pts_output_sptr = dts_pixel_time_series_pca_vnl_vector_fixed
							<unsigned,unsigned,double,128,2>::pca_new_sptr(*pts_ptr);
						break;
					}//end case 2
				case 3:
					{
						pts_output_sptr = dts_pixel_time_series_pca_vnl_vector_fixed
							<unsigned,unsigned,double,128,3>::pca_new_sptr(*pts_ptr);
						break;
					}//end case 3
				default:
					{
						vcl_cerr << "----ERROR---- "
								 << "dts_pixel_time_series_pca_process: Unknown destination dimension, "
								 << " please augment.\n"
								 << "\tFILE: " << __FILE__ << '\n'
								 << "\tLINE: " << __LINE__ << '\n'
								 << vcl_flush;
						return false;
					}//end default

				}//end switch(dest_dim)
				break;
			}//end case 128
		default:
			{
				vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series_pca_process: Unknown Source dimension, "
                         << " please augment.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                         << vcl_flush;
                return false;
			}//end default
		}//end swtich(src_dim)

		pro.set_output_val(0,pts_output_sptr);
		return true;
	}//end if pixel_type == unsigned && time_type == unsigned && element_type == double

	vcl_cerr << "----ERROR---- "
		<< "dts_pixel_time_series_pca_process:"
		<< " you shouldn't be here given conditionals.\n"
		<< "\tFILE: " << __FILE__ << '\n'
		<< "\tLINE: " << __LINE__ << '\n'
		<< vcl_flush;
	return false;
}//end dts_pixel_time_series_pca_process