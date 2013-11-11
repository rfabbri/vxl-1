//this is /contrib/bm/dts/pro/processes/dts_pixel_time_series_create_sptr_process.cxx
//:
// \file
// \date October 22, 2011
// \author Brandon A. Mayer
//
// Create an empty instance of a dts_pixel_time_series_base_sptr pointing to
// the appropriate concrete child.
//
// Inputs: 
//         1. pixel type (vcl_string)
//         2. time type (vcl_string)
//         3. element type (vcl_string)
//         4. dimension (unsigned)
//
// Outputs:
//         1. dts_pixel_time_series_base_sptr
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<dts/dts_pixel_time_series_base_sptr.h>
#include<dts/dts_pixel_time_series.h>

namespace dts_pixel_time_series_create_sptr_process_globals
{
    const unsigned int n_inputs_ = 4;
    const unsigned int n_outputs_ = 1;
}

//set input/outputs
bool dts_pixel_time_series_create_sptr_process_cons( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_create_sptr_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    vcl_vector<vcl_string> output_types_(n_outputs_);

    unsigned i = 0;
    input_types_[i++] = "vcl_string";//pixel type
    input_types_[i++] = "vcl_string";//time type
    input_types_[i++] = "vcl_string";//element type
    input_types_[i++] = "unsigned"; //dimension

    output_types_[0] = "dts_pixel_time_series_base_sptr";

    if( !pro.set_input_types(input_types_) )
        return false;

    if( !pro.set_output_types(output_types_) )
        return false;

    return true;
}

bool dts_pixel_time_series_create_sptr_process( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_create_sptr_process_globals;

    if( pro.n_inputs() < n_inputs_ )
    {
        vcl_cout << pro.name()
                 << " dts_pixel_time_series_write_bin_process: "
                 << " The input number should be: "
                 << n_inputs_ << vcl_endl;
        return false;
    }

    //get inputs
    unsigned i = 0;
    vcl_string pixel_type = pro.get_input<vcl_string>(i++);
    vcl_string time_type = pro.get_input<vcl_string>(i++);
    vcl_string element_type = pro.get_input<vcl_string>(i++);
    unsigned dim = pro.get_input<unsigned>(i++);
    
    dts_pixel_time_series_base_sptr dts_sptr;

    if( pixel_type == "unsigned" && 
        time_type == "unsigned" &&
        element_type == "double")
    {
        switch(dim)
        {
        case 1:
            {
                dts_sptr = 
                    new dts_pixel_time_series<unsigned,unsigned,double,1>();
            }//end case 1
            break;
        case 2:
            {
                dts_sptr = 
                    new dts_pixel_time_series<unsigned,unsigned,double,2>();
            }//end case 2
            break;
        case 3:
            {
                dts_sptr = 
                    new dts_pixel_time_series<unsigned,unsigned,double,3>();
            }//end case 3
            break;
        case 10:
            {
                dts_sptr = 
                    new dts_pixel_time_series<unsigned,unsigned,double,10>();
            }//end case 10
            break;
        case 128:
            {
                dts_sptr = 
                    new dts_pixel_time_series<unsigned,unsigned,double,128>();
            }//end case 128
            break;
        default:
            {
                vcl_cout << "Unhandled dimension. Please augment."
                         << vcl_endl
                         << "FILE: " << __FILE__ << vcl_endl
                         << "LINE: " << __LINE__ << vcl_endl;
                return false;
            }//end default
        }//end switch(dim)
    }
    else
    {
        vcl_cout << "Unknown pixel/time/element types. Please augment." 
                 << vcl_endl
                 << "FILE: " << __FILE__ << vcl_endl
                 << "LINE: " << __LINE__ << vcl_endl;
        return false;
    }

    pro.set_output_val(0,dts_sptr);

    return true;
}