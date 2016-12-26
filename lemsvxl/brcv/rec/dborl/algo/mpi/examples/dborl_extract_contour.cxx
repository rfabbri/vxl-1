//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dborl_extract_contour.h"
#include "dborl_extract_contour_params.h"
#include <vcl_iostream.h>

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <vul/vul_file.h>

#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbsol/dbsol_file_io.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>


//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_extract_contour::parse_command_line(vcl_vector<vcl_string>& argv)
{
  params_->parse_command_line_args(argv);  // parses the input.xml file if its name is passed from command line
  
  //: always print the params file if an executable to work with ORL web interface
  if (!params_->print_params_xml(params_->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params_->print_params_file() << vcl_endl;

  if (params_->exit_with_no_processing() || params_->print_params_only())
    return false;

  //: set the param_file_ variable just in case
  param_file_ = params_->input_param_filename_;

  return true;
}

//: this method is run on each processor
bool dborl_extract_contour::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_extract_contour::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }

  fclose(xmlFile);
  ind_ = parser.get_index();
  
  if (!ind_)
    return false;

  if (!(ind_->get_type() == dborl_index_type::flat_image)) {
    vcl_cout << "Index is not of flat_image type! Exiting!\n";
    return false;
  }
  
  return true;
}

//: this method is run on each processor
bool dborl_extract_contour::parse(const char* param_file)
{
  params_->parse_input_xml();  // the input parameter file name has already been parsed by the parse_command_line_args() method of params
  if (params_->exit_with_no_processing() || params_->print_params_only())
    return false;

  return parse_index(params_->index_filename());
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_extract_contour::print_default_file(const char* def_file)
{
  params_->print_default_input_xml(vcl_string(def_file));
}

//: this method is run on each processor
//  initialize the input vector with indices of objects in the root of the index file
//  assuming a flat index of images so only distributes the objects at the root
bool dborl_extract_contour::initialize(vcl_vector<int>& t)
{
  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  
  //: check if the index contains all the information needed
  if (root->names().size() != root->paths().size()) {
    vcl_cout << "The index does not contain object names as well as object paths! Exiting!\n";
    return false;
  }

  for (unsigned i = 0; i < root->names().size(); i++) {
    t.push_back((int)i);
  }
  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
//  load the image and trace contour
bool dborl_extract_contour::process(int t1, char& f)
{
  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();

  vcl_string img_name = root->paths()[t1] + "/" + root->names()[t1] + params_->image_extension();
  if (!vul_file::exists(img_name)) {
    vcl_cout << "In dborl_extract_contour::process() -- img: " << img_name << " could not be loaded\n";
    f = 0;
    return false;
  }

  vil_image_resource_sptr image_sptr = vil_load_image_resource(img_name.c_str());
  if (!image_sptr) {
    vcl_cout << "In dborl_extract_contour::process() -- problems in loading img: " << img_name << "\n";
    f = 0;
    return false;
  }

  vil_image_view<bool> binary_img;

  // no conversion necessary when input is binary image
  if (image_sptr->pixel_format()==binary_img.pixel_format() && 
    image_sptr->nplanes() == 1)
  {
    binary_img = image_sptr->get_view();
  }
  else
  {
    // get the grey view
    vil_image_view<vxl_byte> image;  
    if (image_sptr->nplanes()==1)
    {
      image = *vil_convert_cast(vxl_byte(), image_sptr->get_view());
    }
    else
    {
      image = *vil_convert_cast(vxl_byte(), 
      vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()));
    }

    // threshold the image to make sure that it is binary
    vxl_byte threshold_value = 128;
    if (params_->intensity_threshold() == -1)
    {
      vxl_byte min_value = 0, max_value = 0;
      vil_math_value_range(image, min_value, max_value);
      threshold_value = (min_value+max_value)/2;
    }
    else if (0<=params_->intensity_threshold() && params_->intensity_threshold()<=255)
    {
      threshold_value = (vxl_byte)(params_->intensity_threshold());
    }
    vil_threshold_above<vxl_byte >(image, binary_img, threshold_value);
  }

  
  //invoke the tracer
  dbdet_contour_tracer ctracer;
  ctracer.set_sigma(params_->sigma());
  ctracer.set_curvature_smooth_nsteps(params_->nsteps());
  ctracer.set_curvature_smooth_beta(params_->curvature_smoothing_beta());
  ctracer.trace(binary_img);
 
  //get the largest contour and save
  vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (ctracer.largest_contour());
  vcl_string output_name;
  if (params_->save_to_output_folder()) 
    output_name = params_->output_directory() + "/" + root->names()[t1] + params_->image_extension();
  else
    output_name = root->paths()[t1] + "/" + root->names()[t1] + params_->image_extension();
  output_name = vul_file::strip_extension(output_name) + ".con";

  if (!dbsol_save_con_file(output_name.c_str(), newContour)) {
    vcl_cout << "In dborl_extract_contour::process() -- problems in saving con file: " << output_name << vcl_endl;
    f = 0;
    return false;
  }

  f = 1;
  return true;
}

bool dborl_extract_contour::finalize(vcl_vector<char>& results)
{
  for (unsigned i = 0; i < results.size(); i++) {
    if (!results[i]) {
      vcl_cout << "Problems occurred processing some of the inputs\n";
      return false;
    }
  }
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_extract_contour::create_datatype_for_R()
{
  return MPI::CHAR;
}
#else
MPI_Datatype dborl_extract_contour::create_datatype_for_R()
{
  return MPI_CHAR;
}
#endif


