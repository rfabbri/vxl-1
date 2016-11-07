// This is brcv/shp/dbskfg/pro/dbskfg_detect_transforms_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_detect_transforms_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
// vidpro1 headers
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>


//: Constructor
dbskfg_detect_transforms_process::dbskfg_detect_transforms_process()
{
    if (
        !parameters()->add( "Detect Gaps  " , "-b_gaps", bool(true)) ||
        !parameters()->add( "Detect Loops " , "-b_loops", bool(true)) ||
        !parameters()->add( "Alpha" , "-alpha" , (double)0.5) || 
        !parameters()->add( "Euler Sprial Completion" , "-ess" , (double)0.25) 
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_detect_transforms_process::~dbskfg_detect_transforms_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_detect_transforms_process::clone() const
{
    return new dbskfg_detect_transforms_process(*this);
}

vcl_string
dbskfg_detect_transforms_process::name()
{
    return "Detect Composite Transforms";
}

vcl_vector< vcl_string >
dbskfg_detect_transforms_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "composite_graph" );
    return to_return;

}

vcl_vector< vcl_string >
dbskfg_detect_transforms_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    //to_return.push_back("vsol2D");
    return to_return;
}

int dbskfg_detect_transforms_process::input_frames()
{
    return 1;
}

int dbskfg_detect_transforms_process::output_frames()
{
    return 1;
}

bool dbskfg_detect_transforms_process::execute()
{
    // get input composite graph storage class
    dbskfg_composite_graph_storage_sptr input_graph;
    input_graph.vertical_cast(input_data_[0][0]);
  
    // get coarse shock graph_process
    dbskfg_composite_graph_sptr composite_graph = 
        input_graph->get_composite_graph();
    dbskfg_rag_graph_sptr rag_graph = 
        input_graph->get_rag_graph();

    bool detect_gaps(true);
    bool detect_loops(true);
    double ess=0.25;
    double alpha=0.5;

    parameters()->get_value( "-b_gaps" ,  detect_gaps );
    parameters()->get_value( "-b_loops" , detect_loops );
    parameters()->get_value( "-ess" ,     ess );
    parameters()->get_value( "-alpha" ,   alpha );
  
    dbskfg_detect_transforms_sptr transformer;

    // Start algorithm
    if ( input_graph->get_transforms() != 0)
    {
        transformer = input_graph->get_transforms();  
    }
    else
    {
        transformer = new dbskfg_detect_transforms(composite_graph,
                                                   input_graph->get_image());
        // Set transformer for image
        input_graph->set_transforms(transformer);
    }

    // Delete transformer's all transforms first off
    transformer->destroy_transforms();

    // Kick of detecting transformers
    transformer->detect_transforms(detect_gaps,
                                   detect_loops,
                                   ess,
                                   alpha);

    // //Create the output storage class
    // vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();

    // //Grab the results
    // vcl_vector<dbskfg_transform_descriptor_sptr> results=transformer->objects();

    // vcl_cout<<"Number of Transforms found: "<<results.size()<<vcl_endl;
    // for ( unsigned int i(0) ; i < results.size()  ; ++i)
    // {
    //     // For each transform lets compute polygon
    //     output_vsol->add_objects(results[i]->contours_spatial_objects_);
    //     output_vsol->add_objects(results[i]->new_contours_spatial_objects_);
    //     results[i]->print(vcl_cout);
    // }

  
    //Set storage with new transform graph
    //Set storage with new transform graph
    clear_output();
    //output_data_[0].push_back(output_vsol);

    

    return true;
}

bool dbskfg_detect_transforms_process::finish()
{
    return true;
}


