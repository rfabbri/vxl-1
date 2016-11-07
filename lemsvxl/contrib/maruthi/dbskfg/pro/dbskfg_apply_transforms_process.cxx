// This is brcv/shp/dbskfg/pro/dbskfg_apply_transforms_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_apply_transforms_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>

//: Constructor
dbskfg_apply_transforms_process::dbskfg_apply_transforms_process()
{
    if (
        !parameters()->add( "Threshold:  " , "-thresh", double(0.5))
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_apply_transforms_process::~dbskfg_apply_transforms_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_apply_transforms_process::clone() const
{
  return new dbskfg_apply_transforms_process(*this);
}

vcl_string
dbskfg_apply_transforms_process::name()
{
  return "Apply Composite Transforms";
}

vcl_vector< vcl_string >
dbskfg_apply_transforms_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "composite_graph" );
  return to_return;

}

vcl_vector< vcl_string >
dbskfg_apply_transforms_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}

int dbskfg_apply_transforms_process::input_frames()
{
  return 1;
}

int dbskfg_apply_transforms_process::output_frames()
{
  return 1;
}

bool dbskfg_apply_transforms_process::execute()
{
    // get input composite graph storage class
    dbskfg_composite_graph_storage_sptr input_graph;
    input_graph.vertical_cast(input_data_[0][0]);
  
    // get coarse shock graph_process
    dbskfg_composite_graph_sptr composite_graph = 
        input_graph->get_composite_graph();
    dbskfg_rag_graph_sptr rag_graph =
        input_graph->get_rag_graph();
    dbskfg_detect_transforms_sptr transformer=
        input_graph->get_transforms();

    double thresh=0.15;
    parameters()->get_value( "-thresh" ,  thresh );
  
    transformer->apply_transforms(thresh,rag_graph);
  
    //Set storage with new transform graph
    clear_output();

    return true;
}

bool dbskfg_apply_transforms_process::finish()
{
  return true;
}


