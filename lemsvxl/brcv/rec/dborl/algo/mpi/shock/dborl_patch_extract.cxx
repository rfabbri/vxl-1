//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08

//
//

#include "dborl_patch_extract.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dborl/algo/dborl_utilities.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <bbas/bsol/bsol_algs.h>
#include <bsold/bsold_file_io.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsold/algo/bsold_curve_algs.h>

#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/algo/dbsk2d_ishock_detector.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbsk2d/dbsk2d_file_io.h>

#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_model_selector.h>
#include <dbskr/pro/dbskr_shock_patch_selector.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_tree.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_patch_extract::parse_command_line(std::vector<std::string>& argv)
{

  if (!params_.parse_command_line_args(argv))
    return false;

  //: always print the params file if an executable to work with ORL web interface
  if (!params_.print_params_xml(params_.print_params_file()))
    std::cout << "problems in writing params file to: " << params_.print_params_file() << std::endl;

  param_file_ = params_.input_param_filename_;
  return true;
}

//: this method is run on each processor
bool dborl_patch_extract::parse(const char* param_file)
{
  params_.input_param_filename_ = param_file; // just in case
  if (!params_.parse_input_xml())
    return false;

  if (params_.exit_with_no_processing() || params_.print_params_only())
    return false;

  return true;
}

//: this method is run on each processor
bool dborl_patch_extract::parse_index(std::string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  std::FILE *xmlFile = std::fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    std::cout << "dborl_shock_retrieval::parse_index() -- " << index_file << "-- error on opening" << std::endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     std::cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << std::endl;
     return 0;
   }

  fclose(xmlFile);
  ind_ = parser.get_index();
  
  if (!ind_)
    return false;

  std::cout << "parsed the index file with name: " << ind_->name_ << std::endl;

  root_ = ind_->root_->cast_to_index_node();
  if (root_->names().size() != root_->paths().size()) {
    std::cout << "dborl_shock_retrieval::parse_index() -- " << index_file << "-- number of names not equal number of paths!!" << std::endl;
    return false;
  }

  return true;
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_patch_extract::print_default_file(const char* def_file)
{
  params_.print_default_input_xml(std::string(def_file));
}

//: this method is run on each processor
bool dborl_patch_extract::initialize(std::vector<dborl_patch_extract_input>& t)
{
  //: parse the index file 
  if (!parse_index(params_.db_index_()))
    return false;

  unsigned D = root_->names().size();
  std::cout << "db size: " << D << std::endl;

  //: initialize the input vector for each pair
  for (unsigned i = 0; i<D; i++) {

    std::string output_name = params_.patch_folder_() + "/" + params_.patch_folder_.file_type() +
              "/" + root_->names()[i] + "/" + root_->names()[i] + params_.extract_patch_.output_file_postfix() + "/" +
              root_->names()[i] + params_.extract_patch_.output_file_postfix() + "-patch_strg.bin";
  
    if (!vul_file::exists(output_name)) {
      dborl_patch_extract_input inp(root_->names()[i], (root_->paths()[i] + "/" + root_->names()[i]));
      t.push_back(inp);
      if (t.size() == total_processors_)
        return true;

    } else {
      std::cout << output_name << " exists, skipping!\n";
    }

  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_patch_extract::process(dborl_patch_extract_input inp, char& f)
{
  dbsk2d_shock_graph_sptr final_sg;
  dbsk2d_xshock_graph_fileio loader;
  vsol_box_2d_sptr box = new vsol_box_2d();;
  std::vector<vsol_spatial_object_2d_sptr> conts;

  std::string esf_file;
  if (params_.use_object_shock_()) {
    esf_file = inp.full_name + ".esf";
    std::string bnd_file = inp.full_name + "_boundary.bnd";
    
    if (!vul_file::exists(esf_file)) {
      std::cout << "In dborl_patch_extract::process() -- cannot find: " << esf_file << std::endl;
      return false;
    }  

    final_sg = loader.load_xshock_graph(esf_file.c_str());

    if (!vul_file::exists(bnd_file)) {
      std::cout << "In dborl_patch_extract::process() -- WARNING cannot find: " << bnd_file << " bounding box won't be computed!" << std::endl;
      std::cout.flush();
    } else {
      //: find the bounding box 
      dbsk2d_file_io::load_bnd_v3_0(bnd_file, conts);
      for (unsigned i = 0; i <conts.size(); i++) {
        if (conts[i]->cast_to_curve()) {
          if (conts[i]->cast_to_curve()->cast_to_line()) 
            conts[i]->cast_to_curve()->cast_to_line()->compute_bounding_box();
          else if (conts[i]->cast_to_curve()->cast_to_polyline())
            conts[i]->cast_to_curve()->cast_to_polyline()->compute_bounding_box();
        } else if (conts[i]->cast_to_region()) {
          if (conts[i]->cast_to_region()->cast_to_polygon())
            conts[i]->cast_to_region()->cast_to_polygon()->compute_bounding_box();
        }
        box->grow_minmax_bounds(conts[i]->get_bounding_box());
      }
    }
  
  } else {

    esf_file = params_.assoc_shock_folder_() + "/" + params_.assoc_shock_folder_.file_type() + "/" + inp.name + "/" + inp.name + ".esf";
    std::string bnd_file = params_.assoc_shock_folder_() + "/" + params_.assoc_shock_folder_.file_type() + "/" + inp.name + "/" + inp.name + "_boundary.bnd";

    if (!vul_file::exists(esf_file)) {
      std::cout << "In dborl_patch_extract::process() -- cannot find: " << esf_file << std::endl;
      return false;
    }  

    final_sg = loader.load_xshock_graph(esf_file.c_str());

    if (!vul_file::exists(bnd_file)) {
      std::cout << "In dborl_patch_extract::process() -- WARNING cannot find: " << bnd_file << " bounding box won't be computed!" << std::endl;
      std::cout.flush();
    } else {
      //: find the bounding box 
      dbsk2d_file_io::load_bnd_v3_0(bnd_file, conts);
      for (unsigned i = 0; i <conts.size(); i++) {
        if (conts[i]->cast_to_curve()) {
          if (conts[i]->cast_to_curve()->cast_to_line()) 
            conts[i]->cast_to_curve()->cast_to_line()->compute_bounding_box();
          else if (conts[i]->cast_to_curve()->cast_to_polyline())
            conts[i]->cast_to_curve()->cast_to_polyline()->compute_bounding_box();
        } else if (conts[i]->cast_to_region()) {
          if (conts[i]->cast_to_region()->cast_to_polygon())
            conts[i]->cast_to_region()->cast_to_polygon()->compute_bounding_box();
        }
        box->grow_minmax_bounds(conts[i]->get_bounding_box());
      }
    }

  }

  //if (!test_shock_graph_for_rec(final_sg)) {
  if (!test_xshock_graph(final_sg)) {
    std::cout << "In dborl_patch_extract::process() -- shock graph: " << esf_file << " failed the test!!\n";
    f = 0;
    return false;
  }

  std::cout << "process: " << inp.name << ", " << final_sg->number_of_vertices() << " vertices in the shock graph\n";
  std::cout.flush();
  
  
  //: extract the patches
  // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();

  std::string output_name_prefix = params_.patch_folder_();
  if (!vul_file::exists(output_name_prefix))
    vul_file::make_directory(output_name_prefix);

  output_name_prefix = output_name_prefix + "/" + params_.patch_folder_.file_type();
  if (!vul_file::exists(output_name_prefix))
      vul_file::make_directory(output_name_prefix);

  output_name_prefix = output_name_prefix + "/" + inp.name + "/";
  if (!vul_file::exists(output_name_prefix))
      vul_file::make_directory(output_name_prefix);

  output_name_prefix = output_name_prefix + inp.name + params_.extract_patch_.output_file_postfix() + "/";
  if (!vul_file::exists(output_name_prefix))
    vul_file::make_directory(output_name_prefix);

  std::string output_name = output_name_prefix + inp.name + params_.extract_patch_.output_file_postfix() + "-patch_strg.bin";
  
  if (vul_file::exists(output_name)) {
    std::cout << "\t" << inp.name << " patch storage exists\n";
    f = 1;
    return true;
  }

  if (params_.extract_patch_.extract_from_tree_()) {
    if (params_.extract_patch_.put_only_tree_()) {
      
      //: use the trees dart count as an estimate of the max possible depth of this shock graph
      dbskr_tree_sptr dummy_tree = new dbskr_tree();
      dummy_tree->acquire(final_sg, false, false, false);  // options are meaningless

      dbskr_shock_patch_model_selector selector(final_sg);
      selector.extract(dummy_tree->size(), params_.extract_patch_.circular_ends_());
      selector.prune_same_patches(dummy_tree->size());
      selector.add_to_storage(dummy_tree->size(), output);
      if (output->size() != 1)
        std::cout << "WARNING: trying to put ONLY tree but there are more than one patches in the storage!!!!!!!!!\n";

    } else {
      dbskr_shock_patch_model_selector selector(final_sg);
    
      for (int d = params_.extract_patch_.start_depth_(); d <= params_.extract_patch_.end_depth_(); d += params_.extract_patch_.depth_interval_()) {
        //std::cout << " depth: " << d << " ..";
        selector.extract(d, params_.extract_patch_.circular_ends_());
        selector.prune_same_patches(d);
        //std::cout << " DONE! ";
      }
      selector.prune_same_patches_at_all_depths();

      for (int d = params_.extract_patch_.start_depth_(); d <= params_.extract_patch_.end_depth_(); d += params_.extract_patch_.depth_interval_()) {
        selector.add_to_storage(d, output);
      }
    }

    output_name_prefix = output_name_prefix + inp.name + params_.extract_patch_.output_file_postfix();
    //vsl_b_ofstream bfs((output_name_prefix + "-patch_strg.bin").c_str());
    vsl_b_ofstream bfs((output_name).c_str());
    output->b_write(bfs);
    bfs.close();

    //std::cout << output->size() << " patches, saving shocks..";
    //: save esfs for each patch to load later
    for (unsigned i = 0; i < output->size(); i++) {
      dbskr_shock_patch_sptr sp = output->get_patch(i);
      std::ostringstream oss;
      oss << sp->id();
      std::string patch_esf_name = output_name_prefix + "-" + oss.str() + ".esf";
      dbsk2d_xshock_graph_fileio file_io;
      file_io.save_xshock_graph(sp->shock_graph(), patch_esf_name);
    }
    //std::cout << output->size() << " DONE!\n";

  } else {

    if (!(box->area() > 0)) {
      std::cout << " In dborl_patch_extract::process() -- box area is zero, quitting!\n";
      f = 0;
      return false;
    }
        
    std::string image_file = inp.full_name + params_.image_extention_();
    std::string kept_dir_name = output_name_prefix + "kept/";
    vul_file::make_directory_path(kept_dir_name);
    std::string discarded_dir_name = output_name_prefix + "discarded/";
    vul_file::make_directory_path(discarded_dir_name);

    if (!find_shock_patches(image_file, final_sg, box, kept_dir_name, discarded_dir_name, output_name, 
      params_.extract_patch_.contour_ratio_(), params_.extract_patch_.circular_ends_(), 
      params_.extract_patch_.area_threshold_ratio_(), params_.extract_patch_.overlap_threshold_(), 
      params_.extract_patch_.start_depth_(), params_.extract_patch_.end_depth_(), params_.extract_patch_.depth_interval_(), 
      params_.extract_patch_.pruning_depth_(), params_.extract_patch_.sort_threshold_(), params_.extract_patch_.keep_pruned_(),
      params_.extract_patch_.save_images_(), params_.extract_patch_.save_discarded_images_())) {
      std::cout << " In dborl_patch_extract::process() -- problems in find_shock_patches method, quitting!\n";
      f = 0;
      return false;
    }
  }
 
  f = 1;
  return true;
}

void dborl_patch_extract::print_time()
{
  std::cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  std::cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_patch_extract::finalize(std::vector<char>& results)
{
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_patch_extract::create_datatype_for_R()
{
  return MPI::CHAR;
}
#else
MPI_Datatype dborl_patch_extract::create_datatype_for_R()
{
  return MPI_CHAR;
}
#endif

