// This is brcv/shp/dbskfg/pro/dbskfg_read_training_process.cxx

//:
// \file

#include <dbskfg/pro/dbskfg_read_training_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbxml/dbxml_algos.h>
#include <bxml/bxml_read.h>

//: Constructor
dbskfg_read_training_process::dbskfg_read_training_process()
{

    if( !parameters()->add( "Training file <filename...>" , "-training_input" , 
                            bpro1_filepath("","*.xml") ) )
    {
        std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
    }


}

//: Destructor
dbskfg_read_training_process::~dbskfg_read_training_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_read_training_process::clone() const
{
    return new dbskfg_read_training_process(*this);
}

std::string
dbskfg_read_training_process::name()
{
    return "Load Training File";
}

std::vector< std::string >
dbskfg_read_training_process::get_input_type()
{
    std::vector< std::string > to_return;
    to_return.push_back( "composite_graph" );
    return to_return;
}

std::vector< std::string >
dbskfg_read_training_process::get_output_type()
{
    std::vector< std::string > to_return;
    return to_return;
}

int dbskfg_read_training_process::input_frames()
{
    return 1;
}

int dbskfg_read_training_process::output_frames()
{
    return 1;
}

bool dbskfg_read_training_process::execute()
{

    // get input composite graph storage class
    dbskfg_composite_graph_storage_sptr input_graph;
    input_graph.vertical_cast(input_data_[0][0]);
  
    // get coarse shock graph_process
    dbskfg_composite_graph_sptr composite_graph = 
        input_graph->get_composite_graph();

    // Get transformer object
    dbskfg_detect_transforms_sptr transformer=
       input_graph->get_transforms();

    // get input file path
    bpro1_filepath input;
    parameters()->get_value( "-training_input" , input);
    std::string input_file_path = input.path;

    // read document
    bxml_document doc_in = bxml_read(input_file_path);
    bxml_data_sptr root_xml = doc_in.root_element();
 
    std::vector<unsigned int> transform_ids;
    std::vector<bool> class_ids;
    std::vector<bxml_data_sptr> transform_data; 
    dbxml_algos::find_all_elems_by_name(root_xml,"transform",transform_data);

    for ( unsigned int i=0; i < transform_data.size(); ++i)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((transform_data[i]).ptr());
        unsigned int id(0);
        bool flag=false;

        data_elm->get_attribute("id",id);
        data_elm->get_attribute("class",flag);
        //std::cout<<"id: "<<id<<" class: "<<flag<<std::endl;
        transform_ids.push_back(id);
        class_ids.push_back(flag);
    }

    // Get transformer
    clear_output();

    bxml_data_sptr write_xml=new bxml_element("transform_training");
    std::string training_filename="/home/mn/Desktop/training_costs/xml_files/temp.xml";

    // Grab all transform objects
    std::vector<dbskfg_transform_descriptor_sptr> objects = 
        transformer->objects();

    for ( unsigned int t=0; t < transform_ids.size() ; ++t)
    {
        objects[transform_ids[t]]->write_transform(write_xml,
                                                   training_filename,
                                                   class_ids[t]);

    }
    return true;
}

bool dbskfg_read_training_process::finish()
{
    return true;
}


