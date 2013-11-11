// This is /lemsvxl/brcv/seg/dbdet/pro/dbdet_kovesi_edge_linker_process.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 2, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "dbdet_kovesi_edge_linker_process.h"
#include <dbdet/algo/dbdet_kovesi_edge_linker.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <vcl_string.h>

//: Constructor
dbdet_kovesi_edge_linker_process::dbdet_kovesi_edge_linker_process()
{
    if (    !parameters()->add( "Temporary location for storing kovesi octave files:"   , "-temp_path" , vcl_string("/vision/scratch/octave_tmp/kovesi")) ||
            !parameters()->add( "Edge strength threshold (0-255)" , "-edge_threshold" , 15 ) ||
            !parameters()->add( "Linking edge length threshold" , "-edge_length" , 4 ) )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbdet_kovesi_edge_linker_process::~dbdet_kovesi_edge_linker_process()
{
}


//: Clone the process
bpro1_process* dbdet_kovesi_edge_linker_process::clone() const
{
    return new dbdet_kovesi_edge_linker_process(*this);
}


//: Return the name of this process
vcl_string dbdet_kovesi_edge_linker_process::name()
{
    return "Kovesi Edge Linker (Octave)";
}


//: Return the number of input frame for this process
int dbdet_kovesi_edge_linker_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int dbdet_kovesi_edge_linker_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_kovesi_edge_linker_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_kovesi_edge_linker_process::get_output_type()
{
    vcl_vector<vcl_string > to_return;
    //output the sel storage class
    to_return.push_back( "sel" );

    return to_return;
}


//: Execute the process
bool dbdet_kovesi_edge_linker_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbdet_kovesi_edge_linker_process::::execute() - not exactly one input \n";
        return false;
    }
    clear_output();

    //get the parameters
    int edge_threshold, edge_length;
    vcl_string kovesi_temp_dir;

    parameters()->get_value( "-temp_path", kovesi_temp_dir);
    parameters()->get_value( "-edge_threshold", edge_threshold);
    parameters()->get_value( "-edge_length", edge_length);

    //get the input storage class
    dbdet_edgemap_storage_sptr input_edgemap;
    input_edgemap.vertical_cast(input_data_[0][0]);
    dbdet_edgemap_sptr in_EM = input_edgemap->get_edgemap();

    vcl_cout << "EM: Height:" << in_EM->height() << " Width:" << in_EM->width() << vcl_endl;

    // create the sel storage class
    dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();
    dbdet_curve_fragment_graph& out_CFG = output_sel->CFG();
    dbdet_edgemap_sptr* out_EM = new dbdet_edgemap_sptr;

    dbdet_kovesi_edge_linker kel(kovesi_temp_dir);
    kel.link_and_prune_edges(in_EM, edge_threshold, edge_length, *out_EM, out_CFG);
    output_sel->set_EM(*out_EM);

    // output the sel storage class
    output_data_[0].push_back(output_sel);

    return true;
}

bool dbdet_kovesi_edge_linker_process::finish()
{
    return true;
}
