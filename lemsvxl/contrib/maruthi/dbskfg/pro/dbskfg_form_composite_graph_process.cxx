// This is brcv/shp/dbskfg/pro/dbskfg_form_composite_graph_process.cxx

//:
// \file

#include <dbskfg/pro/dbskfg_form_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/algo/dbskfg_compute_composite_graph.h>
#include <dbskfg/algo/dbskfg_compute_composite_graph_streamlined.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_composite_graph.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

//: Constructor
dbskfg_form_composite_graph_process::dbskfg_form_composite_graph_process(
    bool adjacency_flag)
    :adjacency_flag_(adjacency_flag)
{
   if (
       !parameters()->add( "Construct locus  " , "-locus", bool(true))
       )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}

//: Destructor
dbskfg_form_composite_graph_process::~dbskfg_form_composite_graph_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_form_composite_graph_process::clone() const
{
    return new dbskfg_form_composite_graph_process(*this);
}

vcl_string
dbskfg_form_composite_graph_process::name()
{
    return "Compute Composite Graph";
}

vcl_vector< vcl_string >
dbskfg_form_composite_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "shock" );
    to_return.push_back( "image" );
    return to_return;
}

vcl_vector< vcl_string >
dbskfg_form_composite_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "composite_graph" );
    return to_return;
}

int dbskfg_form_composite_graph_process::input_frames()
{
    return 1;
}

int dbskfg_form_composite_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_form_composite_graph_process::execute()
{
    // 1) get input storage class
    dbsk2d_shock_storage_sptr shock_storage; 
    shock_storage.vertical_cast(input_data_[0][0]);

    // 2) get input storage classes
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][1]);

    bool construct_locus(true);

    parameters()->get_value( "-locus" ,  construct_locus );

    /*********************** Compute Composite Graph ************************/

    // Create new composite graph
    dbskfg_composite_graph_sptr composite_graph = 
        new dbskfg_composite_graph(construct_locus);

    // Create new composite graph
    dbskfg_rag_graph_sptr rag_graph = 
        new dbskfg_rag_graph();

    //Start algorithm
    // dbskfg_compute_composite_graph cgraph_former(shock_storage->
    //                                              get_ishock_graph(),
    //                                              shock_storage->
    //                                              get_shock_graph(),
    //                                              composite_graph,
    //                                              rag_graph);
    // bool status = cgraph_former.compile_composite_graph();
    
    // Start algorithm
    dbskfg_compute_composite_graph_streamlined cgraph_simple(shock_storage->
                                                             get_ishock_graph(),
                                                             composite_graph,
                                                             rag_graph,
                                                             adjacency_flag_);
    bool status = cgraph_simple.compile_composite_graph(shock_storage->
                                                        get_boundary());

    if ( !status )
    {
        return status;
    }

    // Create the output storage class
    dbskfg_composite_graph_storage_sptr output_composite_graph =
        dbskfg_composite_graph_storage_new();

    // Put out data
    output_composite_graph->set_composite_graph(composite_graph);
    output_composite_graph->set_rag_graph(rag_graph);
    
    if (frame_image)
    {
        vil_image_resource_sptr image_sptr = frame_image->get_image();
        output_composite_graph->set_image(image_sptr);
    }

    clear_output();
    output_data_[0].push_back(output_composite_graph);

    return status;
}

bool dbskfg_form_composite_graph_process::finish()
{
    return true;
}


