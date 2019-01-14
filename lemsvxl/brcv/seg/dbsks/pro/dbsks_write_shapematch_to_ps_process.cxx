// This is dbsks/pro/dbsks_write_shapematch_to_ps_process.cxx

//:
// \file

#include "dbsks_write_shapematch_to_ps_process.h"

#include <bpro1/bpro1_parameters.h>
#include <cstdio>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsks/dbsks_utils.h>
#include <bsold/bsold_file_io.h>
#include <bsold/io/bsold_file_io_extras.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbsksp/dbsksp_xshock_node.h>

#include <sstream>



//: Constructor
dbsks_write_shapematch_to_ps_process::
dbsks_write_shapematch_to_ps_process()
{
  std::vector<std::string > opt_mode_desc;
  if ( !parameters()->add("process file?" , "process_file", true) ||
    !parameters()->add("shapematch output file " , "shapematch_file", 
    bpro1_filepath("*.*", "")) ||
    
    !parameters()->add("image file " , "image_file", 
    bpro1_filepath("*.*", "")) ||
    
    !parameters()->add("Output PS file: " , "out_ps_file", 
    bpro1_filepath("*.ps", "") ) ||

    !parameters()->add("process folder? " , "process_folder", false) ||

    !parameters()->add("image list" , "image_list", 
    bpro1_filepath("*.*", "")) ||

    !parameters()->add("shapematch output folder " , "shapematch_folder", 
    bpro1_filepath("*.*", "")) ||
    
    !parameters()->add("image folder " , "image_folder", 
    bpro1_filepath("", "")) ||

    !parameters()->add("image extension " , "image_ext", std::string(".jpg")) ||
    
    !parameters()->add("Output PS folder " , "out_ps_folder", 
    bpro1_filepath("", "") )
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbsks_write_shapematch_to_ps_process::
~dbsks_write_shapematch_to_ps_process()
{
}


//: Clone the process
bpro1_process* dbsks_write_shapematch_to_ps_process::
clone() const
{
  return new dbsks_write_shapematch_to_ps_process(*this);
}

//: Returns the name of this process
std::string dbsks_write_shapematch_to_ps_process::
name()
{ 
  return "Write shapematch to PS"; 
}

//: Provide a vector of required input types
std::vector< std::string > dbsks_write_shapematch_to_ps_process::
get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "dbsksp_shock" );
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbsks_write_shapematch_to_ps_process::
get_output_type()
{
  std::vector<std::string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_write_shapematch_to_ps_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_write_shapematch_to_ps_process::
output_frames()
{
  return 1;
}



//: Execute this process
bool dbsks_write_shapematch_to_ps_process::
execute()
{
  if ( input_data_.size() != 1 )
  {
    std::cerr << "In dbsks_write_shapematch_to_ps_process::execute() - "
             << "not exactly one input images" << std::endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------

  /////  FILE
  bool process_file = false;
  this->parameters()->get_value("process_file", process_file);
 
  bpro1_filepath shapematch_file;
  this->parameters()->get_value("shapematch_file", shapematch_file);
 
  bpro1_filepath image_file;
  this->parameters()->get_value("image_file", image_file);

  bpro1_filepath out_ps_file;
  this->parameters()->get_value("out_ps_file", out_ps_file);


  ////// FOLDER
  bool process_folder = false;
  this->parameters()->get_value("process_folder", process_folder);

  bpro1_filepath image_list;
  this->parameters()->get_value("image_list", image_list);

  bpro1_filepath shapematch_folder;
  this->parameters()->get_value("shapematch_folder", shapematch_folder);

  bpro1_filepath image_folder;
  this->parameters()->get_value("image_folder", image_folder);

  std::string image_ext;
  this->parameters()->get_value("image_ext", image_ext);

  bpro1_filepath out_ps_folder;
  this->parameters()->get_value("out_ps_folder", out_ps_folder);
  
  // STORAGE CLASSES ----------------------------------------------------------
  
  // Shock graph
  dbsksp_shock_storage_sptr shock_storage;
  shock_storage.vertical_cast(input_data_[0][0]);

  // PROCESS DATA -------------------------------------------------------------

  // reference shock graph
  dbsksp_shock_graph_sptr graph = shock_storage->shock_graph();

  if (process_file)
  {
    std::cout << "Processing image : " << vul_file::strip_directory(image_file.path) << "...\n";
    if (this->save_ps_file(out_ps_file.path, shapematch_file.path, image_file.path, graph))
    {
      std::cout << "  Saving PS file succeeded.\n";
    }
    else
    {
      std::cout << "  Saving PS file failed.\n";
    }
  }


  // Process the whole folder
  if (process_folder)
  {
    // parse image list
    std::vector<std::string > image_names;

    std::ifstream ifs(image_list.path.c_str());
    if (!ifs) 
    {
      std::cout << "ERROR: Unable to open file list " << image_list.path.c_str() << std::endl;
      return false;
    }

    // read the image names, one by one
    while (!ifs.eof()) 
    {
      std::string name;
      ifs >> name;
      if (name.size() > 0) 
      {
        std::string just_name = 
          vul_file::strip_directory(vul_file::strip_extension(name));
        image_names.push_back(just_name);
      }
    }
    ifs.close();


    // some how finished parsing
    for (unsigned i =0; i < image_names.size(); ++i)
    {
      std::string image_name = image_names[i];
      std::cout << "\nProcessing image : " << image_name << "...";

      // path to the file
      std::string image_file = image_folder.path + "/" + image_name + image_ext;

      // path to shapematch file
      std::string shapematch_file = shapematch_folder.path + "/" + image_name + 
        "_shapematch_out.txt";

      // path to ps file
      std::string ps_file = out_ps_folder.path + "/" + image_name + "_shapematch_out.ps";

      // Write out the file
      if (this->save_ps_file(ps_file, shapematch_file, image_file, graph))
      {
        std::cout << "  succeeded.\n";
      }
      else
      {
        std::cout << "  failed.\n";
      }

      //// debugging
      //std::cout << "  image_file = " << image_file << "\n"
      //  << "  shapematch_file = " << shapematch_file << "\n"
      //  << "  ps_file = " << ps_file << "\n";
    }
  }

  return false;
}


// ----------------------------------------------------------------------------
bool dbsks_write_shapematch_to_ps_process::
finish()
{
  return true;
}







// ----------------------------------------------------------------------------
bool dbsks_write_shapematch_to_ps_process::
save_ps_file(const std::string& out_ps_file,
             const std::string& shapematch_file, 
              const std::string& image_file,
             const dbsksp_shock_graph_sptr& graph)
{
  vil_image_resource_sptr image_resource = 
    vil_load_image_resource( image_file.c_str() );

  if (!image_resource)
  {
    std::cout << "ERROR: could not load image file.\n";
    return false;
  }

  // variables we want to extract info from
  double bbox_xmin = -2e10;
  double bbox_xmax = -2e10;
  double bbox_ymin = -2e10;
  double bbox_ymax = -2e10;
  int num_nodes = -1;

  // parse the input data file
  std::ifstream in_file(shapematch_file.c_str());
  while (!in_file.eof())
  {
    char buffer[1000];
    in_file.getline(buffer, 1000);

    char tag[1000] = "";
    char data[1000];
    std::sscanf(buffer, "%s %s\n", tag, data);
    std::string tag_string(tag);

    //std::cout << "tag = " << tag_string << "\n  value = " << data << "\n";

    std::stringstream str;
    str << data;

    // determine action depending name of tag
    if (tag_string == "bbox_xmin")
    {
      float value;
      str >> value;
      bbox_xmin = value;
    }
    else if (tag_string == "bbox_ymin")
    {
      float value;
      str >> value;
      bbox_ymin = value;
    }
    else if (tag_string == "bbox_xmax")
    {
      float value;
      str >> value;
      bbox_xmax = value;
    }
    else if (tag_string == "bbox_ymax")
    {
      float value;
      str >> value;
      bbox_ymax = value;
    }
    else if (tag_string == "number_of_nodes")
    {
      float value;
      str >> value;
      num_nodes = int(value);
      break;
    }
  }

  if (bbox_xmin < -1e10 || bbox_xmax < -1e10 || bbox_ymin < -1e10 || bbox_ymax < -1e10 
    || num_nodes < 0)
  {
    std::cout << "ERROR: missing items in shapematch_file.\n";
    return false;
  }

  if (num_nodes > 100)
  {
    std::cout << "WARNING: num_nodes is larger than 100. Is it correct?\n";
  }

  std::map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
  for (int i =0; i < num_nodes; ++i)
  {
    char buffer[1000];
    in_file.getline(buffer, 1000);

    int id;
    double x[5];
    std::sscanf(buffer, "%d %lf %lf %lf %lf %lf\n", &id, &x[0], &x[1], &x[2], &x[3], &x[4]);
    dbsksp_xshock_node_descriptor xnode(x[0], x[1], x[2], x[3], x[4]);
    dbsksp_shock_node_sptr node = graph->node_from_id(id);
    xnode_map.insert(std::make_pair(node, xnode));
    
    //std::cout << "id = " << id << "\n";
    //xnode.print(std::cout );
  }

  // trace out the boundary and contact shocks
  std::vector<vsol_spatial_object_2d_sptr > bnd_list = 
    dbsks_trace_boundary(graph, xnode_map);
  std::vector<vsol_spatial_object_2d_sptr > contact_shock_list = 
    dbsks_trace_contact_shocks(graph, xnode_map);


  // trace out the bounding box
  std::vector<vsol_point_2d_sptr > bbox_pts;
  bbox_pts.push_back(new vsol_point_2d(bbox_xmin, bbox_ymin));
  bbox_pts.push_back(new vsol_point_2d(bbox_xmax, bbox_ymin));
  bbox_pts.push_back(new vsol_point_2d(bbox_xmax, bbox_ymax));
  bbox_pts.push_back(new vsol_point_2d(bbox_xmin, bbox_ymax));

  vsol_polygon_2d_sptr bbox = new vsol_polygon_2d(bbox_pts);



  // combine the two and assign colors for each
  std::vector<vsol_spatial_object_2d_sptr > vsol_data;
  vsol_data.reserve(bnd_list.size() + contact_shock_list.size());
  std::vector<vil_rgb<float > > colors;
  colors.reserve(bnd_list.size() + contact_shock_list.size());

  // blue for boundary
  for (unsigned i =0; i < bnd_list.size(); ++i)
  {
    vsol_data.push_back(bnd_list[i]);
    colors.push_back(vil_rgb<float >(0.0f, 0.0f, 1.0f));
  }

  // green for contact shock
  for (unsigned i =0; i < contact_shock_list.size(); ++i)
  {
    vsol_data.push_back(contact_shock_list[i]);
    colors.push_back(vil_rgb<float >(0.0f, 1.0f, 0.0f));
  }

  // red for bounding box
  vsol_data.push_back(bbox.as_pointer());
  colors.push_back(vil_rgb<float >(1.0f, 0.0f, 0.0f));
  
  // write the file
  return bsold_save_ps_file(out_ps_file, image_resource, vsol_data, colors);
}





