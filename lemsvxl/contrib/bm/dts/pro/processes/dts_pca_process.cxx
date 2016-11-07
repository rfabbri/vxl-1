//this is /contrib/bm/dts/pro/processes/dts_pca_process.cxx
//:
// \file
// \date October 7, 2011
// \author Brandon A. Mayer
//
// Process to reduce dimensionality of features in dts_time_series
// This process will modify the underlying data
// For a process that will produce a new smart pointer but leave the
// input intact, see dts_pca_new_process
//
// Inputs: 1. dts_time_series_base_sptr
//         2. time type (string)
//         3. math type (string)
//		   4. original dimension (unsigned)
//         5. reduced dimension (unsigned)
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dts/dts_time_series_base_sptr.h>
#include<dts/algo/dts_time_series_pca_vnl_vector_fixed.h>

//: global variables
namespace dts_pca_process_globals
{
    const unsigned int n_inputs_ = 5;
    const unsigned int n_outputs_ = 0;
}

bool dts_pca_process_cons( bprb_func_process& pro )
{
    //set input/output types
    
    using namespace dts_pca_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);

    unsigned i = 0;
    input_types_[i++] = "dts_time_series_base_sptr";
    input_types_[i++] = "string";
    input_types_[i++] = "string";
    input_types_[i++] = "unsigned";
    input_types_[i++] = "unsigned";

    if(!pro.set_input_types(input_types_))
        return false;

    return true;
}//end dts_pca_process_cons

bool dts_pca_process( bprb_func_process& pro )
{
    using namespace dts_pca_process_globals;

    if( pro.n_inputs() < n_inputs_ )
    {
        vcl_cerr << "----ERROR---- " 
                 << pro.name() << '\n'
                 << "\tdts_pca_process: The input number should be: "
                 << n_inputs_ << '\n'
                 << "\tFILE: " << __FILE__ << '\n'
                 << "\tLINE: " << __LINE__ << '\n'
                 << vcl_flush;
    }

    //get inputs
    unsigned i = 0;
    dts_time_series_base_sptr ts_sptr = 
        pro.get_input<dts_time_series_base_sptr>(i++);

    vcl_string time_type =
        pro.get_input<vcl_string>(i++);

    vcl_string element_type = 
        pro.get_input<vcl_string>(i++);

    unsigned src_dim = 
        pro.get_input<unsigned>(i++);

    unsigned dest_dim =
        pro.get_input<unsigned>(i++);

    if( time_type == "unsigned" && element_type == "double" )
    {
        switch(src_dim)
        {
        case 10:
            {
                switch(dest_dim)
                {
                case 2:
                    {
                        dts_time_series_pca_vnl_vector_fixed
                            <unsigned,double,10,2>::pca(ts_sptr);
                        return true;
                        break;
                    }//end case 2
                default:
                    {
                        vcl_cerr << "----ERROR---- "
                         << "dts_pca_process: Unknown Destination dimension, "
                         << " please augment.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                         << vcl_flush;
                        return false;
                    }//end default
                }//end switch(dest_dim)
                break;
            }
        case 128:
            {
                switch(dest_dim)
                {
                case 2:
                    {
                        dts_time_series_pca_vnl_vector_fixed
                            <unsigned,double,128,2>::pca(ts_sptr);
                        return true;
                        break;
                    }//end case 2
                }//end switch(dest_dim)
                break;
            }
        default:
            {
                vcl_cerr << "----ERROR---- "
                         << "dts_pca_process: Unknown Source dimension, "
                         << " please augment.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                         << vcl_flush;
                return false;
            }
        }//end swtich
    }
    else if( time_type == "unsigned" && element_type == "float" )
    {
        vcl_cerr << "----ERROR---- NOT YET IMPLEMENTED!"
                 << "\tFLIE: " << __FILE__ << '\n'
                 << "\tLINE: " << __LINE__ << '\n'
                 << vcl_flush;
        return false;
    }
    else
    {
        vcl_cerr << "----ERROR ---- "
                 << "dts_pca_process: Unknown time/element type.\n"
                 << "\tFILE: " << __FILE__ << '\n'
                 << "\tLINE: " << __LINE__ << '\n'
                 << vcl_flush;
        return false;
    }

    return true;
}//end dts_pca_process