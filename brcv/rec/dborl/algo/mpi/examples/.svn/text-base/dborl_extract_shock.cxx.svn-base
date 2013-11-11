//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dborl_extract_shock.h"
#include "dborl_extract_shock_params.h"

#include <vcl_iostream.h>

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <vul/vul_file.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbsol/dbsol_file_io.h>
#include <dbsol/algo/dbsol_curve_algs.h>
//#include <dbskr/dbskr_utilities.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_file_io.h>

#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/algo/dbsk2d_ishock_detector.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <bbas/bsol/bsol_algs.h>

#include <dbskr/algo/dbskr_rec_algs.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_extract_shock::parse_command_line(vcl_vector<vcl_string>& argv)
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
bool dborl_extract_shock::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_extract_shock::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
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

  return true;
}

//: this method is run on each processor
bool dborl_extract_shock::parse(const char* param_file)
{
  params_->parse_input_xml();  // the input parameter file name has already been parsed by the parse_command_line_args() method of params
  if (params_->exit_with_no_processing() || params_->print_params_only())
    return false;

  return parse_index(params_->index_filename());
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_extract_shock::print_default_file(const char* def_file)
{
  params_->print_default_input_xml(vcl_string(def_file));
}

//: this method is run on each processor
//  initialize the input vector with indices of objects in the root of the index file
//  assuming a flat index of images so only distributes the objects at the root
bool dborl_extract_shock::initialize(vcl_vector<int>& t)
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
//  load the contour file and extract shock
bool dborl_extract_shock::process(int t1, char& f)
{
  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();

  vcl_string contour_name;
  if (params_->use_assoc_contours())
    //contour_name = params_->input_contour_dir() + "/" + params_->input_contour_dir.file_type() + "/" + root->names()[t1] + "/" + root->names()[t1] + params_->input_extension();
    contour_name = params_->input_contour_dir() + "/" + root->names()[t1] + "/" + root->names()[t1] + params_->input_extension();
  else
    contour_name = root->paths()[t1] + "/" + root->names()[t1] + params_->input_extension();

  if (!vul_file::exists(contour_name)) {
    vcl_cout << "In dborl_extract_shock::process() -- contour: " << contour_name << " could not be loaded\n";
    f = 0;
    return false;
  } 

  //vcl_cout << "In dborl_extract_shock::process() -- processing: " << contour_name << "\n";

  vcl_string out_name;
  
  if (params_->save_to_output_folder()) {
    out_name = params_->output_directory() + "/" + params_->output_directory.file_type() + "/";
    if (!vul_file::exists(out_name))
      vul_file::make_directory(out_name);
  } else
    out_name = root->paths()[t1] + "/";
  
  out_name = out_name + root->names()[t1] + "/";

  if (!vul_file::exists(out_name)) 
    vul_file::make_directory(out_name);
  
  vcl_string out_shock_name = out_name + root->names()[t1] + params_->output_extension();
  
  if (vul_file::exists(out_shock_name)) {
    vcl_cout << root->names()[t1] << " exists\n";
    f = 1;
    return true;
  }
  
  vcl_cout << "extracting: " << root->names()[t1] << "\n";
  vcl_vector<vsol_spatial_object_2d_sptr> conts_temp;

  vcl_vector<vsol_spatial_object_2d_sptr> conts;
  
  if (params_->input_simple_closed_contours()) {  // input is in .con format

    vcl_vector<vsol_point_2d_sptr> points;
    bool is_closed;
    if (!dbsol_load_con_file(contour_name.c_str(), points, is_closed)) {
      vcl_cout << "In dborl_extract_shock::process() -- contour: " << contour_name << " could not be loaded\n";
      f = 0;
      return false;
    }

    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
    conts_temp.push_back(poly->cast_to_spatial_object());

  } else { // input is a .cem file
     // load the cem file
     
     if (!dbsol_load_cem(conts_temp, contour_name)) {
      vcl_cout << "In dborl_extract_shock::process() -- contour cem: " << contour_name << " could not be loaded\n";
      f = 0;
      return false;
     }
  }

  dbsk2d_shock_graph_sptr sampled_sg;

  bool first_time = true;
  float fit_lines_rms = params_->extract_shock_.fit_lines_rms_();
  int cnt = 0;
  while (!test_shock_graph_for_rec(sampled_sg)) {  
    vcl_cout << "\ttrial: " << cnt << vcl_endl;

    conts.clear();
   if ((first_time && params_->extract_shock_.fit_lines_()) || !first_time) {
     vcl_cout << "\t\tfitting lines, rms: " << fit_lines_rms << vcl_endl;
     
     for (unsigned kk = 0; kk < conts_temp.size(); kk++) {
       if (conts_temp[kk]->cast_to_region()) {
         if (conts_temp[kk]->cast_to_region()->cast_to_polygon()) {
           vsol_polygon_2d_sptr fitted_poly = fit_lines_to_contour(conts_temp[kk]->cast_to_region()->cast_to_polygon(), (double)fit_lines_rms);
           if (fitted_poly)
            conts.push_back(fitted_poly->cast_to_spatial_object());
         }
       } else if (conts_temp[kk]->cast_to_curve()) {
         if (conts_temp[kk]->cast_to_curve()->cast_to_polyline()) {
           vsol_polyline_2d_sptr fitted_poly = fit_lines_to_contour(conts_temp[kk]->cast_to_curve()->cast_to_polyline(), (double)fit_lines_rms);
           if (fitted_poly)
            conts.push_back(fitted_poly->cast_to_spatial_object());
         }
       } else {
         vcl_cout << "In dborl_extract_shock::process() -- WARNING: cem file contains type other than  polygon and polyline!!:" << contour_name << " skipping the object with this type in this cem file\n";
       }
     }
   } else {
     conts.insert(conts.begin(), conts_temp.begin(), conts_temp.end());
   }

   if (!params_->input_simple_closed_contours() && params_->extract_shock_.add_bounding_box_()) {
     vsol_box_2d_sptr bbox = new vsol_box_2d();
     for (unsigned kk = 0; kk < conts.size(); kk++) {
       if (conts[kk]->cast_to_region()) {
         if (conts[kk]->cast_to_region()->cast_to_polygon()) {
           conts[kk]->cast_to_region()->cast_to_polygon()->compute_bounding_box();
           bbox->grow_minmax_bounds(conts[kk]->get_bounding_box());
         }
       } else if (conts[kk]->cast_to_curve()) {
         if (conts[kk]->cast_to_curve()->cast_to_polyline()) {
           conts[kk]->cast_to_curve()->cast_to_polyline()->compute_bounding_box();
           bbox->grow_minmax_bounds(conts[kk]->get_bounding_box());
         }
       }
     }
     if (bbox->area() > 0) {
      int offset = params_->extract_shock_.bounding_box_offset_();
      bbox->add_point(bbox->get_min_x()-offset, bbox->get_min_y()-offset);
      bbox->add_point(bbox->get_max_x()+offset, bbox->get_max_y()+offset);  // enlarge the box with 3 pixels
      vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
      conts.push_back(box_poly->cast_to_spatial_object());
     }
   }

   first_time = false;
   fit_lines_rms += 0.001;
   cnt++;

   if (fit_lines_rms > 0.1 || cnt > 100) {
    vcl_cout << "\tfit lines rms threshold exceeded 0.1, or cnt exceeded 100, quitting!!\n";
    f = 0;
    return false;
   }
  

  // save the boundary curves used in shock extraction
 
  dbsk2d_file_io::save_bnd_v3_0(out_name+root->names()[t1]+"_boundary.bnd", conts);

  // compute shocks ---------------------------------------------------------------------------------------------------
  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(conts, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
  dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
  if (!isg) {
    vcl_cout << "\tproblems in intrinsic shock computation!!\n";
    f = 0;
    continue;
  }
  dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
  dbsk2d_prune_ishock ishock_pruner(isg, sg);
  ishock_pruner.prune(params_->extract_shock_.prune_threshold_());  // prune threshold is 1.0f
  ishock_pruner.compile_coarse_shock_graph();
  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    vcl_cout << "\tsampled shock graph has zero vertices or edges!!\n";
    f = 0;
    continue;
  }

  dbsk2d_sample_ishock ishock_sampler(sg);
  ishock_sampler.sample(params_->extract_shock_.sampling_ds_(), 2);  // sample both sides, if only sample inside == set option to 1
  sampled_sg = ishock_sampler.extrinsic_shock_graph();

  }

  // -------------------------------------------------------------------------------------------------------------------
  dbsk2d_xshock_graph_fileio writer;
  writer.save_xshock_graph(sampled_sg, out_shock_name);
  
  f = 1;
  return true;
}

bool dborl_extract_shock::finalize(vcl_vector<char>& results)
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
MPI::Datatype dborl_extract_shock::create_datatype_for_R()
{
  return MPI::CHAR;
}
#else
MPI_Datatype dborl_extract_shock::create_datatype_for_R()
{
  return MPI_CHAR;
}
#endif


