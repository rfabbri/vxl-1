// This is contrib/fine/dbseg_pro/dbseg_seg_load_sequence_process.cxx

#include <pro/dbseg_seg_load_sequence_process.h>
#include <pro/dbseg_seg_load_process.h>
#include <bpro1/bpro1_parameters.h>
#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_storage_sptr.h>
#include <vul/vul_file.h>

#include <iostream>


//: Constructor
dbseg_seg_load_sequence_process::
dbseg_seg_load_sequence_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Segmentation folder:" , "-seg_folder", bpro1_filepath("","*") ) ||
      !parameters()->add( "Segmentation list file:" , "-seg_list", bpro1_filepath("","*") )
        )
    {
        std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
    }
}


//: Destructor
dbseg_seg_load_sequence_process::
~dbseg_seg_load_sequence_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_load_sequence_process::clone() const
{
    return new dbseg_seg_load_sequence_process(*this);
}


//: Return the name of the process
std::string dbseg_seg_load_sequence_process::
name()
{
    return "Load Segmentation Sequence";
}


//: Call the parent function and reset num_frames_
void dbseg_seg_load_sequence_process::
clear_output(int resize)
{
  num_frames_ = 0;
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
std::vector< std::string > dbseg_seg_load_sequence_process::
get_input_type()
{
    std::vector< std::string > to_return;
    // no input type required
    to_return.clear();
    return to_return;
}


//: Returns a vector of strings describing the output types of this process
std::vector< std::string >dbseg_seg_load_sequence_process::
get_output_type()
{
    std::vector< std::string > to_return;
    to_return.push_back( "seg" );
    return to_return;
}


//: Returns the number of input frames to this process
int dbseg_seg_load_sequence_process::
input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int dbseg_seg_load_sequence_process::
output_frames()
{
    return num_frames_;
}


//: Run the process on the current frame
bool dbseg_seg_load_sequence_process::
execute()
{
  // 1. parse input parameters

  // seg folder
  bpro1_filepath seg_folder_path;
  this->parameters()->get_value("-seg_folder" , seg_folder_path);
  std::string seg_folder = seg_folder_path.path;

  // be smart: in case user enters a filename instead of a folder, extract the
  // folder containing the file
  if (!vul_file::is_directory(seg_folder))
  {
    seg_folder = vul_file::dirname(seg_folder);
  }

  // seg name list
  bpro1_filepath seg_list_path;
  this->parameters()->get_value("-seg_list" , seg_list_path);
  //std::string seg_list = seg_list_path.path;

  // parse the seg_list file to extract the list of file names
  std::vector<std::string > seg_names;
  
  // 2. parse the seg_list file to extract the names
  std::ifstream fp(seg_list_path.path.c_str());
  if (!fp) 
  {
    std::cout<<"ERROR: Can't open " << seg_list_path.path << std::endl;
    return false;
  }

  while (!fp.eof()) 
  {
    std::string name;
    fp >> name;
    if (!name.empty())
    {
      seg_names.push_back(name);
    }
  }
  fp.close();


  std::cout << "Found " << seg_names.size() << " structures." << std::endl;
  // 3. load segs in the files and push to the repository
  for (unsigned i = seg_names.size(); i > 0; --i)
  {
    // load the seg
    std::string seg_file = seg_folder + "/" + seg_names[i-1];

    //vil_image_resource_sptr image_resource = vil_load_image_resource(image_file.c_str());
    
    dbseg_seg_object_base* seg_base = dbseg_seg_load_process::static_execute(seg_file);
    if (seg_names.size() > 9) {
        if (i % (seg_names.size() / 10) == 0) {
            std::cout << i / (seg_names.size() / 10) << "%..";
        }
    }
    if (!seg_base)
    {
      std::cerr << "ERROR: can't load seg " << seg_file << ". Process canceled.\n";
      this->output_data_.clear();
      this->num_frames_ = 0;
      return false;
    }

    // place holder for the image
    dbseg_seg_storage_sptr seg_storage = dbseg_seg_storage_new();
    seg_storage->add_object(seg_base);
    //image_storage->set_image( image_resource );
    output_data_.push_back(std::vector< bpro1_storage_sptr >(1,seg_storage));
    this->num_frames_++;
  }
  return true;   
}


//: Finish
bool dbseg_seg_load_sequence_process::
finish() 
{
  return true;
}





