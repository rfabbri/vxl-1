// This is rec/dborl/algo/mpi/symseg/mpi_compute_ishock_processor.cxx
//:
// \file

#include "mpi_compute_ishock_processor.h"

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/algo/dbdet_load_edg.h>
#include <dbsol/dbsol_file_io.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>


// -----------------------------------------------------------------------------
//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool mpi_compute_ishock_processor::
parse_command_line(vcl_vector<vcl_string>& argv)
{
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> input_xml_file(arg_list, "-x", 
    "the input file in xml format that sets parameters of the algorithm","");
  vul_arg<bool> print_usage_only(arg_list, "-usage", "print usage info and exit", false);
  vul_arg<bool> print_help(arg_list, "-help", "print usage info and exit", false);
  vul_arg<bool> print_def_xml(arg_list, "-print-def-xml", 
    "print input.xml file with default params and exit",false);  
  vul_arg_include(arg_list);
  //vul_arg_parse(argc, argv, false); // warn_about_unrecognized_arguments = false

  for (unsigned i = 1; i < argv.size(); ++i) 
  {
    vcl_string arg = argv[i];
    if (arg == vcl_string ("-x")) 
    { 
      input_xml_file.value_ = argv[++i]; 
    } 
    else if (arg == vcl_string ("-usage")) 
    { 
      print_usage_only.value_ = true; 
    }
    else if (arg == vcl_string ("-help")) 
    { 
      print_help.value_ = true; 
    }
    else if (arg == vcl_string ("-print-def-xml")) 
    { 
      print_def_xml.value_ = true; 
    }
  }

  if (print_help() || print_usage_only()) 
  {
    arg_list.display_help();
    return false; // --> to exit
  }

  if (print_def_xml()) 
  {
    vcl_string input = "input.xml";
    mpi_compute_ishock_processor::print_default_file(input.c_str());
    return false;  // --> to exit
  }

  if (input_xml_file().compare("") == 0) 
  {
    arg_list.display_help();
    vcl_cout << "XML filename is empty \n";
    return false;  // --> to exit
  }

  this->param_file_ = input_xml_file();
  return true;  
}



// -----------------------------------------------------------------------------
//: this method is run on each processor
bool mpi_compute_ishock_processor::
parse(const char* param_file)
{
  this->param_doc_ = bxml_read(param_file);
  
  //
  if (!param_doc_.root_element())
  {
    vcl_cout << "ERROR reading parameter XML file: "
      << param_file << "\n";
    vcl_cout << "root = " << param_doc_.root_element() << "\n";
    return false;
  }
  
  if (param_doc_.root_element()->type() != bxml_data::ELEMENT) 
  {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  return this->params_.parse_from_data(param_doc_.root_element());
}



// -----------------------------------------------------------------------------
//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void mpi_compute_ishock_processor::
print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("mpi_compute_ishock_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}


// -----------------------------------------------------------------------------
//: this method is run on each processor
bool mpi_compute_ishock_processor::initialize(
  vcl_vector<mpi_compute_ishock_processor_input>& t)
{
  t_.mark();

  vcl_string file_list = this->params_.file_list();
  vcl_string out_dir = this->params_.out_dir();
  
  vcl_vector<vcl_string> image_names;
 
  if (!parse_strings_from_file(file_list, image_names)) 
  {
    vcl_cout << "Unable to open list file! " << file_list << "!\n";
    return false;
  }

  bxml_document doc;
  bxml_element * root = new bxml_element("mpi_compute_ishock_processor");
  doc.set_root_element(root);
  bxml_element * data1 = params_.create_document_data();
  root->append_data(data1);
  root->append_text("\n");
  bxml_write(out_dir + this->processor_name() + "_using_params.xml", doc);
   
  for (unsigned int i = 0; i<image_names.size(); i++) 
  {
    mpi_compute_ishock_processor_input inp(image_names[i]);
    t.push_back(inp);
  }

  return true;
}



// -----------------------------------------------------------------------------
//: this method is run in a distributed mode on each processor on the cluster
bool mpi_compute_ishock_processor::
process(mpi_compute_ishock_processor_input input, float& f)
{
  vcl_cout << "processing: " << input.image_name_ << " ";

  // we will put all the data in one folder, including the original image
  vcl_string object_dir = this->params_.out_dir() + "/" + input.image_name_;
  
  // if the folder already exists, clean it up
  if (vul_file::exists(object_dir))
  {
    vcl_string all_files = object_dir + "/*";
    //vul_file::delete_file_glob(all_files.c_str());
  }
  // otherwise create it
  {
    vul_file::make_directory(object_dir);
  }

  // copy the image to the object folder
  vcl_string source_image_file = this->params_.file_dir() + "/" + 
    input.image_name_ + this->params_.extension();

  
  // Proces 0
  // save to the object folder
  vcl_string image_file = object_dir + "/" + input.image_name_ + this->params_.extension();
  if (!this->copy_image(source_image_file, image_file))
  {
    vcl_cout << "ERROR: didn't finish copy_image()\n";
    return false;
  }


  // Process I
  vcl_string edgemap_file = object_dir + "/" + input.image_name_ + ".edg";
  if (!this->compute_edgemap_color(image_file, edgemap_file))
  {
    vcl_cout << "ERROR: didn't finish compute_edgemap()\n";
    return false;
  }

  // Process II
  vcl_string cem_file = object_dir + "/" + input.image_name_ + ".cem";
  if (! this->link_edges(edgemap_file, cem_file))
  {
    vcl_cout << "ERROR: didn't finish link_edges()\n";
    return false;
  }

  // Process III
  vcl_string esf_file = object_dir + "/" + input.image_name_ + ".esf";
  if (! this->compute_ishock(cem_file, esf_file))
  {
    vcl_cout << "ERROR: didn't finish compute_ishock()\n";
    return false;
  }

  f = 0.0f;  
  return true;
}



// -----------------------------------------------------------------------------
//: 
bool mpi_compute_ishock_processor::
copy_image(const vcl_string& input_file, const vcl_string& output_file)
{
  vil_image_resource_sptr image = vil_load_image_resource(input_file.c_str());
  if (!vil_save_image_resource(image, output_file.c_str()))
  {
    return false;
  }
  return true;
}


// -----------------------------------------------------------------------------
//: 
bool mpi_compute_ishock_processor::
compute_edgemap(const vcl_string& input_file, const vcl_string& output_file)
{
  // Process I : Third order edge detection
  dbdet_third_order_edge_detector_process toe_process;
  set_process_parameters_of_bpro1(this->params_, toe_process, "third_order_edge_detector");

  // >> input
  //load the input image

  if (!vul_file::exists(input_file)) 
  {
    vcl_cout << "Cannot find image file: " << input_file << "\n";
    return false;
  }

  vil_image_resource_sptr img = vil_load_image_resource(input_file.c_str());
  if (!img) 
  {
    vcl_cout << "Cannot load image: " << input_file << "\n";
    return false;
  }
  vidpro1_image_storage_sptr input_image_storage = new vidpro1_image_storage();
  input_image_storage->set_image(img);

  toe_process.add_input(input_image_storage);

  // >> execute
  toe_process.execute();
  vcl_cout << " processed..";

  // >> finish
  toe_process.finish();
  vcl_cout << " finalized..\n";

  // >> output
  //save this edge map onto a file
  dbdet_edgemap_storage_sptr edgemap_storage; 
  edgemap_storage.vertical_cast(toe_process.get_output()[0]);
  
  if (!edgemap_storage) 
  {
    vcl_cout << "Process output cannot be cast to an edge_map\n";
    return false;
  }
  dbdet_edgemap_sptr edgemap = edgemap_storage->get_edgemap();

  if (!dbdet_save_edg(output_file, edgemap)) 
  {
    vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
    return false;
  }
  return true;
}



// -----------------------------------------------------------------------------
//: 
bool mpi_compute_ishock_processor::
compute_edgemap_color(const vcl_string& input_file, const vcl_string& output_file)
{
  // Process I : Third order edge detection
  dbdet_third_order_color_edge_detector_process toce_process;
  set_process_parameters_of_bpro1(this->params_, toce_process, "third_order_color_edge_detector");

  // force the bLoadComps variable to FALSE. We only load color images
  toce_process.parameters()->set_value( "-bLoadComps", false);

  // >> input
  //load the input image

  if (!vul_file::exists(input_file)) 
  {
    vcl_cout << "Cannot find image file: " << input_file << "\n";
    return false;
  }

  vil_image_resource_sptr img = vil_load_image_resource(input_file.c_str());
  if (!img) 
  {
    vcl_cout << "Cannot load image: " << input_file << "\n";
    return false;
  }
  vidpro1_image_storage_sptr input_image_storage = new vidpro1_image_storage();
  input_image_storage->set_image(img);

  toce_process.add_input(input_image_storage);

  // >> execute
  toce_process.execute();
  vcl_cout << " processed..";

  // >> finish
  toce_process.finish();
  vcl_cout << " finalized..\n";

  // >> output
  //save this edge map onto a file
  dbdet_edgemap_storage_sptr edgemap_storage; 
  edgemap_storage.vertical_cast(toce_process.get_output()[0]);
  
  if (!edgemap_storage) 
  {
    vcl_cout << "Process output cannot be cast to an edge_map\n";
    return false;
  }
  dbdet_edgemap_sptr edgemap = edgemap_storage->get_edgemap();

  if (!dbdet_save_edg(output_file, edgemap)) 
  {
    vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
    return false;
  }
  return true;
}


//: ----------------------------------------------------------------------------
//: 
bool mpi_compute_ishock_processor::
link_edges(const vcl_string& input_file, const vcl_string& output_file)
{
  // Edge linking
  // >> Initialize process
  dbdet_generic_linker_process pro;
  set_process_parameters_of_bpro1(this->params_, pro, "generic_linker");
  pro.parameters()->set_value("-output_vsol", true);


  //load the input edge map
  if (!vul_file::exists(input_file)) 
  {
    vcl_cout << "Cannot find edg file: " << input_file << "\n";
    return false;
  }

  dbdet_edgemap_sptr edge_map;
  if (!dbdet_load_edg(input_file, true, 1.0, edge_map)) 
  {
    vcl_cout << "Cannot load edg file: " << input_file << "\n";
    return false;
  }

  dbdet_edgemap_storage_sptr edgemap_sto = dbdet_edgemap_storage_new();
  edgemap_sto->set_edgemap(edge_map);

  pro.add_input(edgemap_sto);
  pro.execute();
  vcl_cout << " processed..";
  pro.finish();
  vcl_cout << " finalized..\n";

  //:get the output
  vcl_vector<bpro1_storage_sptr> out = pro.get_output();
  if (out.size() != 1) 
  {
    vcl_cout << "Process output does not contain a boundary map\n";
    return false;
  }

  //save this edge map onto a file
    
  vidpro1_vsol2D_storage_sptr output_vsol;
  output_vsol.vertical_cast(out[0]);
  if (!output_vsol) 
  {
    vcl_cout << "Process output cannot be cast to a vsol storage\n";
    return false;
  }

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = output_vsol->all_data();
  if (!dbsol_save_cem(vsol_list, output_file)) 
  {
    vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
    return false;
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Compute intrinsic shocks 
bool mpi_compute_ishock_processor::
compute_ishock(const vcl_string& input_file, const vcl_string& output_file)
{

  // Edge linking
  // >> Initialize process
  dbsk2d_compute_ishock_process pro;
  set_process_parameters_of_bpro1(this->params_, pro, "compute_ishock");

  //load the input edge map
  if (!vul_file::exists(input_file)) 
  {
    vcl_cout << "Cannot find .cem file: " << input_file << "\n";
    return false;
  }


  // Load the boundary from the boundary file
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
  if (!dbsol_load_cem(vsol_list, input_file))
  {
    vcl_cout << "Cannot load the boundary .cem file " << input_file;
    return false;
  }

  // put the boundary into a vsol2D storage
  vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
  vsol_storage->add_objects(vsol_list, "boundary");

  // empty image storage
  vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();


  // Input: set the input for the process
  pro.add_input(image_storage);
  pro.add_input(vsol_storage);
  
  // Execute
  pro.execute();
  vcl_cout << " processed..";

  // Finalize
  pro.finish();
  vcl_cout << " finalized..\n";


  // Output: extract the output shock storage
  vcl_vector<bpro1_storage_sptr> out = pro.get_output();
  if (out.size() != 1) 
  {
    vcl_cout << "ERROR: size of process output is not 1.\n";
    return false;
  }

  // shock storage
  dbsk2d_shock_storage_sptr output_shock_storage;
  output_shock_storage.vertical_cast(out[0]);
  if (!output_shock_storage) 
  {
    vcl_cout << "Process output cannot be cast to a dbsk2d_shock_storage\n";
    return false;
  }
  
  // sample the shock to get an extrinsic shock
  float sampling_resolution = 1.0f;
  bool sample_both_side = true;
  bool sample_inside = true;

  // sample the intrinsic shock graph into an extrinsic shock graph
  dbsk2d_sample_ishock ishock_sampler(output_shock_storage->get_shock_graph());
  if (sample_both_side)
  {
    ishock_sampler.sample(sampling_resolution, BOTHSIDE);
  }
  else if (sample_inside)
  {
    ishock_sampler.sample(sampling_resolution, INSIDE);
  }
  else
  {
    ishock_sampler.sample(sampling_resolution, OUTSIDE);
  }

  // Save the shock graph
  dbsk2d_xshock_graph_fileio xshock_io;
  if (!xshock_io.save_xshock_graph(ishock_sampler.extrinsic_shock_graph(), output_file))
  {
    vcl_cout << "ERROR: Problems in saving esf file: " << output_file 
      << ".Quit now.\n";
    return false;
  }
  return true;
}




// -----------------------------------------------------------------------------
//: 
void mpi_compute_ishock_processor::
print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}


// -----------------------------------------------------------------------------
//: this method is run on the lead processor once after results are collected from each processor
bool mpi_compute_ishock_processor::
finalize(vcl_vector<float>& results)
{
  vcl_cout << "results size: " << results.size() << " not doing anything with them\n";
  for (unsigned i = 0; i < results.size(); i++) {
    vcl_cout << results[i];
  }
  vcl_cout << vcl_endl;
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype mpi_compute_ishock_processor::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype mpi_compute_ishock_processor::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif


