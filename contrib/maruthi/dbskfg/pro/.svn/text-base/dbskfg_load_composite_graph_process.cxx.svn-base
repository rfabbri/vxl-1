// This is brcv/shp/dbskfg/pro/dbskfg_load_composite_graph_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_load_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>


//: Constructor
dbskfg_load_composite_graph_process::dbskfg_load_composite_graph_process()
{
    if( !parameters()->add( "Input file <filename...>" , "-cginput" , 
                            bpro1_filepath("","*.xml") ) )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_load_composite_graph_process::~dbskfg_load_composite_graph_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_load_composite_graph_process::clone() const
{
    return new dbskfg_load_composite_graph_process(*this);
}

vcl_string
dbskfg_load_composite_graph_process::name()
{
    return "Load Composite Graph";
}

vcl_vector< vcl_string >
dbskfg_load_composite_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;

}

vcl_vector< vcl_string >
dbskfg_load_composite_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back("composite_graph");
    return to_return;
}

int dbskfg_load_composite_graph_process::input_frames()
{
    return 1;
}

int dbskfg_load_composite_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_load_composite_graph_process::execute()
{
    // get input file path
    bpro1_filepath input;
    parameters()->get_value( "-cginput" , input);
    vcl_string input_file_path = input.path;

    int num_of_files = 0;

    output_data_.clear();

    // make sure that input_file_path is sane
    if (input_file_path == "") 
    { return false; }

    // Create new composite graph
    dbskfg_composite_graph_sptr composite_graph = 
        new dbskfg_composite_graph();

    // Create new composite graph
    dbskfg_rag_graph_sptr rag_graph = 
        new dbskfg_rag_graph();

    dbskfg_composite_graph_fileio fileio;
    fileio.load_composite_graph(composite_graph,input_file_path);

    // Create the output storage class
    dbskfg_composite_graph_storage_sptr output_composite_graph =
        dbskfg_composite_graph_storage_new();

    // Put out data
    output_composite_graph->set_composite_graph(composite_graph);
    output_composite_graph->set_rag_graph(rag_graph);


    //Set storage with new transform graph
    clear_output();
    output_data_[0].push_back(output_composite_graph);

    return true;
}

bool dbskfg_load_composite_graph_process::finish()
{
    return true;
}


