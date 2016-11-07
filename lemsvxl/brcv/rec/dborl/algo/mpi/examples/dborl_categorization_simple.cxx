//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dborl_categorization_simple.h"
#include "dborl_categorization_simple_params.h"
#include <vcl_iostream.h>

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_mask_description.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <vul/vul_file.h>

#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbsol/dbsol_file_io.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbcvr/dbcvr_clsd_cvmatch.h>
#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>

#include <dborl/dborl_evaluation.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_categorization_simple::parse_command_line(vcl_vector<vcl_string>& argv)
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
bool dborl_categorization_simple::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_categorization_simple::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
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

  vcl_cout << "parsed the index file with name: " << ind_->name_ << vcl_endl;
  //: set the current node to the root
  current_node_ = ind_->root_->cast_to_index_node();

  return true;
}

//: this method is run on each processor
bool dborl_categorization_simple::parse(const char* param_file)
{
  params_->parse_input_xml();  // the input parameter file name has already been parsed by the parse_command_line_args() method of params
  if (params_->exit_with_no_processing())
    return false;

  //: load the input shock graph and create the tree
  vcl_string input_file;
  if (params_->use_shock_matching())
    input_file = params_->input_object_path() + "/" + params_->input_object_name() + ".esf";
  else
    input_file = params_->input_object_path() + "/" + params_->input_object_name() + ".con";

  if (!vul_file::exists(input_file)) {
    vcl_cout << "dborl_categorization_simple::parse() -- cannot find the input file: " << input_file << "\n";
    return false;
  }

  if (params_->use_shock_matching()) {  // load the .esf
    dbsk2d_xshock_graph_fileio loader;
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(input_file.c_str());
    if (!sg) {
      vcl_cout << "dborl_categorization_simple::parse() -- Problems in loading shock graph: "<< input_file << vcl_endl;
      return false;
    } 
      
    input_shock_tree_ = new dbskr_tree(params_->scurve_sample_ds(), params_->scurve_interpolate_ds());
    input_shock_tree_->acquire(sg, params_->elastic_splice_cost(), params_->circular_ends(), params_->combined_edit());
  } else {  // load the .con
    vcl_vector<vsol_point_2d_sptr> points;
    bool is_closed;
    if (!dbsol_load_con_file(input_file.c_str(), points, is_closed) || !points.size()) {
      vcl_cout << "dborl_categorization_simple::process() -- Problems in loading the con file: "<< input_file << vcl_endl;
      return false;
    }
    input_poly_ = new vsol_polygon_2d(points);
    vcl_vector<vsol_point_2d_sptr> flipped_points;
    for (int i = points.size(); i >= 0; i--)
      flipped_points.push_back(points[i]);
    input_poly_flipped_ = new vsol_polygon_2d(flipped_points);
  }

  //: load the ground truth for the input object for comparison to the categorization result
  vcl_string obj_groundtruth = params_->input_object_path() + "/" + params_->input_object_name() + ".xml";
  if (!vul_file::exists(obj_groundtruth)) {
    vcl_cout << "dborl_categorization_simple::parse() -- cannot find ground truth description for input object: " << obj_groundtruth << vcl_endl;
    return false;
  }

  // assuming its an image ground truth description:
  dborl_image_desc_parser parser;
  dborl_image_description_sptr object_desc = dborl_image_description_parse(obj_groundtruth, parser);
  if (!object_desc->has_single_category()) {
    vcl_cout << "dborl_categorization_simple::parse() - WARNING: image description: " << obj_groundtruth << " has more than one category! ambiguity in category comparison! using the first category\n";
  }
  input_gt_category_ = object_desc->get_first_category();

  vcl_cout << "input category: " << input_gt_category_ << "\n";

  return parse_index(params_->index_filename());
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_categorization_simple::print_default_file(const char* def_file)
{
  params_->print_default_input_xml(vcl_string(def_file));
}

//: this method is run on each processor
//  initialize the input vector with names of input esfs in the current node of the index file
bool dborl_categorization_simple::initialize(vcl_vector<vcl_string>& t)
{
  if (!current_node_)
    return false;
  
  //: check if the index contains all the information needed
  if (current_node_->names().size() != current_node_->paths().size()) {
    vcl_cout << "The index does not contain object names as well as object paths! Exiting!\n";
    current_node_->write_xml(vcl_cout);
    return false;
  }

  for (unsigned i = 0; i < current_node_->names().size(); i++) {
    //vcl_string input_name = current_node_->paths()[i] + "/" + current_node_->names()[i] + params_->input_extension();
    vcl_string input_name;
    if (params_->use_shock_matching())
      input_name = current_node_->paths()[i] + "/" + current_node_->names()[i] + ".esf";
    else
      input_name = current_node_->paths()[i] + "/" + current_node_->names()[i] + ".con";
      
    t.push_back(input_name);
  }
  vcl_cout << "initialized the node: " << current_node_->name_  << " \n";
  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
//  load the image and trace contour
bool dborl_categorization_simple::process(vcl_string t1, float& f)
{
  if (!vul_file::exists(t1)) {
    vcl_cout << "In dborl_categorization_simple::process() -- the input file: " << t1 << " is not found!\n";
    f = -1;
    return false;
  }

  vcl_string t1_name = vul_file::strip_extension(t1);
  t1_name = vul_file::strip_directory(t1_name);

  if (params_->check_for_same_object()) {
   if (t1_name.compare(params_->input_object_name()) == 0) {
     vcl_cout << "skipping : " << t1_name << " same as the input object!\n";
     f = 1000.0f;  // a very big number
     return true;
   }
  }

  if (params_->use_shock_matching()) {

    //: load the shock graph and create the tree
    dbsk2d_xshock_graph_fileio loader;
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(t1.c_str());
    if (!sg) {
      vcl_cout << "dborl_categorization_simple::process() -- Problems in loading shock graph: "<< t1 << vcl_endl;
      return false;
    } 
      
    dbskr_tree_sptr shock_tree = new dbskr_tree(params_->scurve_sample_ds(), params_->scurve_interpolate_ds());
    if (!shock_tree->acquire(sg, params_->elastic_splice_cost(), params_->circular_ends(), params_->combined_edit())) {
      vcl_cout << "dborl_categorization_simple::process() -- Problems in loading shock graph: "<< t1 << vcl_endl;
      return false;
    }

    vcl_string out_name1 = params_->output_directory() + params_->input_object_name() + "-" + t1_name + ".shgm";
    vcl_string out_name2 = params_->output_directory() + t1_name + "-" + params_->input_object_name() + ".shgm";

    if (!vul_file::exists(out_name1)) {
      dbskr_tree_edit edit(input_shock_tree_, shock_tree, params_->circular_ends(), params_->localized_edit());
      edit.save_path(true);    
      edit.set_curvematching_R(params_->shock_matching_R());
      if (!edit.edit()) {
        vcl_cout << "dborl_categorization_simple::process() -- Problems in editing trees for: " << out_name1 << "\n";
        return false;
      }
      f = edit.final_cost();
      if (params_->save_matching()) 
        edit.write_shgm(out_name1);
    } else {
      dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor();
      sm_cor->read_and_construct_from_shgm(out_name1, false);
      f = sm_cor->final_cost();
    }

    float saved_f = f;
    if (!vul_file::exists(out_name2)) {
      dbskr_tree_edit edit(shock_tree, input_shock_tree_, params_->circular_ends(), params_->localized_edit());
      edit.save_path(true);    
      edit.set_curvematching_R(params_->shock_matching_R());
      if (!edit.edit()) {
        vcl_cout << "dborl_categorization_simple::process() -- Problems in editing trees for: " << out_name1 << "\n";
        return false;
      }
      f = edit.final_cost();
      if (params_->save_matching()) 
        edit.write_shgm(out_name2);
    } else {
      dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor();
      sm_cor->read_and_construct_from_shgm(out_name2, false);
      f = sm_cor->final_cost();
    }

    if (f > saved_f)  // always keep the smaller f as the real answer for similarity of this pair
      f = saved_f;

    if (params_->use_normalized_cost()) {
      if (params_->arclength_normalization()) 
        f = f / (input_shock_tree_->compute_total_reconstructed_boundary_length(params_->circular_ends()) + shock_tree->compute_total_reconstructed_boundary_length(params_->circular_ends()));
      else
        f = f / (input_shock_tree_->total_splice_cost() + shock_tree->total_splice_cost());
    }

  } else {
    //: load the con file
    vcl_vector<vsol_point_2d_sptr> points;
    bool is_closed;
    if (!dbsol_load_con_file(t1.c_str(), points, is_closed) || !points.size()) {
      vcl_cout << "dborl_categorization_simple::process() -- Problems in loading the con file: "<< t1 << vcl_endl;
      return false;
    }
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
    
    //: match both ways, nothing to save as a match file for now
    dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(input_poly_, poly, params_->curve_matching_R(), params_->rms(), params_->template_size());
    d1->setStretchCostFlag(params_->normalized_stretch_cost());   // if false cost = |ds1-ds2| + R|d_theta1-d_theta2| 
    d1->Match();

    int minIndex;
    f = (float)d1->finalBestCostRestrictedStartingPoint(minIndex, params_->rest_curvematch_ratio(), params_->use_normalized_cost());

    float saved_f = f;
    //: match the flipped version of the first curve as well, curve matching is not mirror transform invariant
    dbcvr_clsd_cvmatch_sptr d2 = new dbcvr_clsd_cvmatch(input_poly_flipped_, poly, params_->curve_matching_R(), params_->rms(), params_->template_size());
    d2->setStretchCostFlag(params_->normalized_stretch_cost());   // if false cost = |ds1-ds2| + R|d_theta1-d_theta2| 
    d2->Match();
    f = (float)d2->finalBestCostRestrictedStartingPoint(minIndex, params_->rest_curvematch_ratio(), params_->use_normalized_cost());
    if (f < saved_f)
      f = saved_f;
  }

  return true;
}

//: this method is run on the lead processor once after results are collected from each processor
//  this method decides to make another round of process distribution or not
//  and if so picks the next job grouping by updating the current index node pointer
bool dborl_categorization_simple::finalize(vcl_vector<float>& results)
{
  if (!results.size() || !current_node_)
    return false;      // its over

  //: find the id of the match with the least final cost
  unsigned min_i = 0;
  float min_f = results[0];
  for (unsigned i = 1; i < results.size(); i++) {
    if (results[i] < min_f) {
      min_f = results[i];
      min_i = i;
    }
  }

  //: find the current categorization, i.e. category of the nearest neighbor in this node of the index
  vcl_string obj_groundtruth = current_node_->paths()[min_i] + "/" + current_node_->names()[min_i] + ".xml";
  if (!vul_file::exists(obj_groundtruth)) {
    vcl_cout << "dborl_categorization_simple::finalize() -- cannot find ground truth description: " << obj_groundtruth << vcl_endl;
    return false;
  }

  // assuming its an image ground truth description:
  dborl_image_desc_parser parser;
  dborl_image_description_sptr object_desc = dborl_image_description_parse(obj_groundtruth, parser);
  if (!object_desc->has_single_category()) {
    vcl_cout << "dborl_categorization_simple::finalize() - WARNING: image description: " << obj_groundtruth << " has more than one category! not a good model! using the first category\n";
  }
  current_category_ = object_desc->get_first_category();

  //: calculate the evaluation statistics regarding this categorization
  vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;

  //: first find all the categories encountered in this dataset, initialize its stat pointer to zero
  for (unsigned i = 0; i < current_node_->paths().size(); i++) {
    vcl_string obj_groundtruth = current_node_->paths()[i] + "/" + current_node_->names()[i] + ".xml";
    dborl_image_desc_parser parser;
    dborl_image_description_sptr object_desc = dborl_image_description_parse(obj_groundtruth, parser);
    //: add all the categories in this description
    for (vcl_map<vcl_string, int>::iterator iter = object_desc->category_list_.begin(); iter != object_desc->category_list_.end(); iter++) 
      category_statistics[iter->first] = 0;   // vcl_map makes sure that each category appears only once, even if this statement called more than once for the same category
  }

  category_statistics[input_gt_category_] = 0;  // make sure that its inserted as well
  vcl_cout << "there are: " <<  category_statistics.size() << " categories encountered in this database, or in the input\n";

  //: now we declared an instance from current_category_ --> update the statistics for this category
  vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.find(current_category_);
  it->second = new dborl_exp_stat();

  vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it_gt = category_statistics.find(input_gt_category_);
  it_gt->second = new dborl_exp_stat();

  if (current_category_.compare(input_gt_category_) == 0) { // the same --> TP of current_category_
    it->second->increment_TP();
  } else {
    it->second->increment_FP();                             // not the same --> FP of current_category_
    it_gt->second->increment_FN();                          //              --> FN of ground truth category_ (missed it)
  }

  //: similarly true negative for all other categories.
  for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator itt = category_statistics.begin(); itt != category_statistics.end(); itt++) {
    if (itt == it || itt == it_gt)
      continue;
    itt->second = new dborl_exp_stat();
    itt->second->increment_TN();
  }

  //: write the evaluation result
  vcl_cout << "will write evaluation output to: " << params_->evaluation_file() << vcl_endl;
  params_->print_evaluation_xml(category_statistics, true);  // print_FN is true for this application
  
  //: write the categorization result
  vcl_string output_file = params_->categorization_file();
  vcl_cout << "will write categorization output to: " << params_->categorization_file() << vcl_endl;
  vcl_ofstream of(output_file.c_str());
  if (!of) {
    vcl_cout << "dborl_categorization_simple::finalize() - not able to write output to: " << output_file << vcl_endl;
  } else {
    of << "<type name = \"categorization_result\">\n";
    of << "\t<description>\n";
    of << "\t\t<instance>\n";
    of << "\t\t\t<category>" << current_category_ << "</category>\n";
    of << "\t\t\t<gt_category>" << input_gt_category_ << "</gt_category>\n";
    of << "\t\t\t<similarity>" << min_f << "</similarity>\n";
    of << "\t\t\t<nearest_neighbor>" << current_node_->paths()[min_i] + "/" + current_node_->names()[min_i] << "</nearest_neighbor>\n";
    of << "\t\t</instance>\n";
    of << "\t</description>\n";
    of << "</type>\n";
  }

  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_categorization_simple::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype dborl_categorization_simple::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif


