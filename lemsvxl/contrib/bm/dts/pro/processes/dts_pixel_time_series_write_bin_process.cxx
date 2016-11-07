//this is /contrib/bm/dts/pro/processes/dts_pixel_time_series_write_bin_process.cxx
//:
// \file
// \date October 22, 2011
// \author Brandon A. Mayer
//
// Process write dts_pixel_time_series_base_sptr binary file
//
// Inputs: 
//         1. dts_pixel_time_series_base_sptr
//         2. filename (vcl_string)
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

namespace dts_pixel_time_series_write_bin_process_globals
{
    const unsigned int n_inputs_ = 2;
    const unsigned int n_outputs_ = 0;
}

//set input/output types
bool dts_pixel_time_series_write_bin_process_cons( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_write_bin_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    
    unsigned i = 0;
    input_types_[i++] = "dts_pixel_time_series_base_sptr";
    input_types_[i++] = "vcl_string"; //filename

    if( !pro.set_input_types(input_types_))
        return false;

    return true;
}

bool dts_pixel_time_series_write_bin_process( bprb_func_process& pro )
{
    using namespace dts_pixel_time_series_write_bin_process_globals;

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
    dts_pixel_time_series_base_sptr dts_sptr = pro.get_input<dts_pixel_time_series_base_sptr>(i++);
    vcl_string filename = pro.get_input<vcl_string>(i++);

    vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);

    if(!os)
        return false;

    dts_sptr->b_write(os);

    return true;
}