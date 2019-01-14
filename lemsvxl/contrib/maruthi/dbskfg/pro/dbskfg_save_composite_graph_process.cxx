// This is brcv/shp/dbskfg/pro/dbskfg_save_composite_graph_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_save_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>


//: Constructor
dbskfg_save_composite_graph_process::dbskfg_save_composite_graph_process()
{
    if(!parameters()->add( 
           "Output File Prefix:" ,  "-file_prefix" ,std::string("")) ||
       !parameters()->add( "Output folder:" , 
                           "-output_folder", bpro1_filepath("", "")) ||
       !parameters()->add( 
           "Prune Threshold:" , "-prune_thres" ,(double) 1.0f) ||
       !parameters()->add( 
           "Contour Ratio:" ,  "-con_ratio" ,  (double) 0.5f ))
    {
        std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
    }

}

//: Destructor
dbskfg_save_composite_graph_process::~dbskfg_save_composite_graph_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_save_composite_graph_process::clone() const
{
    return new dbskfg_save_composite_graph_process(*this);
}

std::string
dbskfg_save_composite_graph_process::name()
{
    return "Save Composite Graph";
}

std::vector< std::string >
dbskfg_save_composite_graph_process::get_input_type()
{
    std::vector< std::string > to_return;
    to_return.push_back("composite_graph");
    return to_return;

}

std::vector< std::string >
dbskfg_save_composite_graph_process::get_output_type()
{
    std::vector< std::string > to_return;
    return to_return;
}

int dbskfg_save_composite_graph_process::input_frames()
{
    return 1;
}

int dbskfg_save_composite_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_save_composite_graph_process::execute()
{

    // get input composite graph storage class
    dbskfg_composite_graph_storage_sptr storage;
    storage.vertical_cast(input_data_[0][0]);
  
    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    std::string output_folder = output_folder_filepath.path;

    std::string output_prefix;
    parameters()->get_value("-file_prefix", output_prefix);
    double prune_thres(1.0f);
    parameters()->get_value( "-prune_thres" , prune_thres);
    double contour_ratio(0.5f);
    parameters()->get_value( "-con_ratio" ,  contour_ratio);

    dbskfg_composite_graph_fileio fileio;
    fileio.write_out(storage->get_rag_graph(),
                     storage->get_image(),
                     output_prefix,
                     output_folder,
                     contour_ratio,
                     prune_thres);

    clear_output();
 

    return true;
}

bool dbskfg_save_composite_graph_process::finish()
{
    return true;
}


