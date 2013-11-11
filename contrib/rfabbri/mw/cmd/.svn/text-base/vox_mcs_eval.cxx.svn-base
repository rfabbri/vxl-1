//: 
//\file
//\brief Evaluation two-view curve fragment matching
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 10/14/2009 08:59:24 PM PDT
//
//

#include "vox_mcs_eval.h"
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <dborl/algo/dborl_utilities.h>
#include <dbxml/dbxml_algos.h>
#include <mw/pro/dbmcs_stereo_driver.h>


#define MW_ASSERT(msg, a, b) if ((a) != (b)) { vcl_cerr << (msg) \
  << " -- Fail:" << (a) << " != " << (b) << vcl_endl; exit(1); }

/*
static bool write_stats(const vcl_string &dataset_name, 
    const vcl_vector<dborl_exp_stat> &stats_at_threshold,
    vcl_string fname
    );
    */

static bool 
write_stats_with_params(
    const vcl_string &dataset_name, 
    const vcl_vector<dborl_exp_stat> &stats_at_threshold,
    vcl_vector<vcl_vector<vcl_string> >eval_params_names,
    vcl_vector<vcl_vector<vcl_string> >eval_params_values,
    vcl_string fname
    );

int 
main(int argc, char *argv[]) 
{
  vox_mcs_eval_params_sptr params = 
    new vox_mcs_eval_params("vox_mcs_eval_params");  

  vox_mcs_eval e;
  e.params_ = params;

  if (!params->parse_command_line_args(argc, argv))
    return 1;

  // always print the params file if an executable to work with ORL web 
  // interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cerr << "problems in writing params file to: " 
             << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  // always call this method to actually parse the input parameter file 
  // whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 1;

  // This program runs on a single mcs_stereo_instance. This is be specified in
  // the command line.
  //
  // It loads a ground truth file in addition to what mcs does.
  //
  // It runs 2-view stereo with varying parameters.
  //
  // It then writes TP/FP/TN/FN results into an xml file, for each combination
  // of parameters.
  //
  // TODO
  // - how are the parameter ranges obtained?
  //    A: I will create a special input parameter for a range, i.e., if a
  //    parameter is called tau_distance_ then I also have tau_distance_range_.
  //
  // - how do I know what parameter combination generated a certain eval file (TP/FP/TN/FN) ?
  //    A: need to output a separate file with the thresholds, where the number
  //    of thresholds is the same number of stats in the eval file. This file
  //    has format:
  //
  //    <run_thresholds>
  //      <param name="param_name">
  //        <threshold value="val 1"></threshold>
  //        <threshold value="val 2"></threshold>
  //        ...
  //        <threshold value="val n"></threshold>
  //      </param>
  //    </run_thresholds>
  //
  // I won't use associated files since this is a single node encapsulating stereo
  // matching and evaluation.

  e.read_cams();
  e.read_frame_data();
  e.read_gt();
  e.read_one_mcs_instance();
  e.read_param_lists();

  if ( !vul_file::exists(params->output_mcs_eval_folder_()) ) {
    bool retval  = vul_file::make_directory(params->output_mcs_eval_folder_());
    if (!retval)
      return 1;
  }

  dbmcs_concurrent_stereo_driver s(e.dpath_, e.one_instance_);
  s.set_max_concurrent_matchers(1);
  s.set_use_curvelets(e.use_curvelets_);

  bool retval = s.init();
  MW_ASSERT("Stereo driver init return value", retval, true);

  if (e.use_curvelets_) {
    vcl_cout << "Using curvelets.\n";
    if (e.min_num_inlier_edgels_per_curvelet_list_.size() > 1)
      vcl_cout << "Warning: currently setting only one value of curvelet inlier threshold\n";

    if (!e.min_num_inlier_edgels_per_curvelet_list_.empty())
      s.set_min_num_inlier_edgels_per_curvelet(e.min_num_inlier_edgels_per_curvelet_list_[0]);
  } else {
    vcl_cout << "Not using curvelets.\n";
  }

  vcl_cout << "Start experiments: " << vcl_endl;
  unsigned i=0;
  for (unsigned i1 = 0; i1 < e.distance_threshold_list_.size(); ++i1)
  for (unsigned i2 = 0; i2 < e.dtheta_threshold_list_.size(); ++i2)
  for (unsigned i3 = 0; i3 < e.min_samples_per_curve_frag_list_.size(); ++i3)
  for (unsigned i4 = 0; i4 < e.min_inliers_per_view_list_.size(); ++i4, ++i) {
    vcl_cout << "iteration[" << i << "]:" 
      << i1 << "," << i2 << "," << i3 << "," << i4 << "," << vcl_endl;
    s.set_distance_threshold(e.distance_threshold_list_[i1]);
    s.set_dtheta_threshold(e.dtheta_threshold_list_[i2]);
    if (e.prune_by_length_) {
      vcl_cout << "Prune by length: " << e.min_samples_per_curve_frag_list_[i3] << " pixels" << vcl_endl;
      s.set_min_length_per_curve_frag(e.min_samples_per_curve_frag_list_[i3]);
    } else { // min samples interpreted as length
      s.set_min_samples_per_curve_frag(
          static_cast<unsigned>(e.min_samples_per_curve_frag_list_[i3]));
    }
    s.set_min_inliers_per_view(e.min_inliers_per_view_list_[i4]);
    s.set_min_total_inliers(0); // threshold only later.

    // TODO: support looping over these; perhaps just do it in the shell script?
    s.set_min_first_to_second_best_ratio(e.min_first_to_second_best_ratio_list_[0]);
    s.set_lonely_threshold(static_cast<unsigned>(e.lonely_threshold_list_[0]));
    s.set_min_epiangle(e.min_epiangle_list_[0]);
    s.set_min_epipolar_overlap(e.min_epipolar_overlap_list_[0]);
    s.update_stereo_params();

    retval = s.run(i+1);
    MW_ASSERT("Stereo driver run return value", retval, true);

    // Now compare to ground truth 
    mw_discrete_corresp corr (s.corresp(s.num_corresp()-1));

    if (corr.checksum() != e.gt_.checksum())
      vcl_cout << "Computed and g-truth checksum for corresp don't match\n"
        << corr << vcl_endl << e.gt_ << vcl_endl;

    // Prune at different thresholds and measure the stats.

    // TODO assert gt is compatible with the read-in curves.
    vcl_vector<dborl_exp_stat> stats;
    const unsigned num_its = e.min_total_inliers_list_.size();
    stats.reserve(num_its);

    // parameter values at each evaluation step.
    vcl_vector<vcl_vector<vcl_string> >eval_params_names(num_its);
    vcl_vector<vcl_vector<vcl_string> >eval_params_values(num_its);

    // TODO Reconstruct from ground truth and write this out.

    for (unsigned i5 = 0; i5 < e.min_total_inliers_list_.size(); ++i5) {
      dborl_exp_stat stt;
      corr.threshold_by_cost_lteq(e.min_total_inliers_list_[i5]);
      corr.exp_stats_hitmiss(stt, &e.gt_);

      // Set values in stats, eval_params_names, and eval_params_values

      {
        stats.push_back(stt);
        eval_params_names[i5].resize(1);
        eval_params_names[i5][0] = "min_total_inliers";

        eval_params_values[i5].resize(1);

        {
        vcl_ostringstream strm;
        strm << e.min_total_inliers_list_[i5];
        eval_params_values[i5][0] = strm.str();
        }
      }
    }

    vcl_string param_stamp;

    param_stamp += vcl_string("-distance_");
    {
    vcl_ostringstream strm;
    strm << e.distance_threshold_list_[i1];
    param_stamp += strm.str();
    }
    param_stamp += vcl_string("-dtheta_");
    {
    vcl_ostringstream strm;
    strm << e.dtheta_threshold_list_[i2];
    param_stamp += strm.str();
    }
    param_stamp += vcl_string("-");
    {
    vcl_ostringstream strm;
    strm << e.min_samples_per_curve_frag_list_[i3];
    param_stamp += strm.str() + vcl_string("_length");
    }
    param_stamp += vcl_string("-min_inliers_per_view_");
    {
    vcl_ostringstream strm;
    strm << e.min_inliers_per_view_list_[i4];
    param_stamp += strm.str();
    }

    write_stats_with_params(
        params->dataset_name_(), 
        stats, 
        eval_params_names,
        eval_params_values,
        params->output_mcs_eval_folder_() + vcl_string("stats") + param_stamp 
        + params->output_mcs_eval_extension_());

    if (params->dump_corresps_()) {
      vsl_b_ofstream corr_ofs((
           params->output_mcs_eval_folder_() + vcl_string("corresp") + param_stamp
          + vcl_string(".vsl")).c_str() );
      vsl_b_write(corr_ofs, s.corresp(s.num_corresp()-1));
    }
  }

  return 0;
}


//: This method writes out the evaluation xml file with TP/FP/TN/FN
//
// Input:
//    dataset_name : the name that goes into eval_results's
//    dataset name.
//
//    stats_at_threshold[] : stats for each threshold.
//
// TODO: move to dborl/algo/dborl_utilities
bool 
write_stats(
    const vcl_string &dataset_name, 
    const vcl_vector<dborl_exp_stat> &stats_at_threshold,
    vcl_string fname
    )
{
  vcl_cout<<"Writing evaluation results to " << fname << vcl_endl;

  if (stats_at_threshold.empty()) {
    vcl_cerr << "Error: no stats.\n";
    return false;
  }

 
  // Xml Tree: eval_results->dataset->stats

  // Create root element
  bxml_document  doc;
  bxml_data_sptr root     = new bxml_element("eval_results");
  bxml_element*  root_elm = dbxml_algos::cast_to_element(root,"eval_results");
  doc.set_root_element(root); 
  root_elm->append_text("\n   ");

  // Create query element
  bxml_data_sptr dataset     = new bxml_element("dataset");
  bxml_element*  dataset_elm = 
      dbxml_algos::cast_to_element(dataset,"dataset");
  root_elm->append_data(dataset);
  root_elm->append_text("\n");
  dataset_elm->set_attribute("name",dataset_name);
  dataset_elm->set_attribute("num_positive",
                             stats_at_threshold[0].positive_cnt_);
  dataset_elm->set_attribute("num_negative",
                             stats_at_threshold[0].negative_cnt_);
  dataset_elm->append_text("\n      ");
  
  // Loop over vector of bounding boxes
  
  for (unsigned i = 0; i < stats_at_threshold.size(); ++i) {
    bxml_data_sptr stats     = new bxml_element("stats");
    bxml_element*  stats_elm = dbxml_algos::
        cast_to_element(stats,"stats");

    if ( i > 0 )
    {
        dataset_elm->append_text("   ");
    }
    dataset_elm->append_data(stats);
    dataset_elm->append_text("\n   ");
    
    // Append coordinates
    bxml_data_sptr tp = new bxml_element("TP");
    bxml_data_sptr fp = new bxml_element("FP");
    bxml_data_sptr tn = new bxml_element("TN");
    bxml_data_sptr fn = new bxml_element("FN");
    
    bxml_element* tp_elm=dbxml_algos::cast_to_element(tp,"TP");
    bxml_element* fp_elm=dbxml_algos::cast_to_element(fp,"FP");
    bxml_element* tn_elm=dbxml_algos::cast_to_element(tn,"TN");
    bxml_element* fn_elm=dbxml_algos::cast_to_element(fn,"FN");
   
    vcl_stringstream tpvalue,fpvalue,tnvalue,fnvalue;
    
    tpvalue<<stats_at_threshold[i].TP_;
    fpvalue<<stats_at_threshold[i].FP_;
    tnvalue<<stats_at_threshold[i].TN_;
    fnvalue<<stats_at_threshold[i].FN_;

    stats_elm->append_text("\n         ");
    stats_elm->append_data(tp);
    tp_elm->append_text(tpvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(fp);
    fp_elm->append_text(fpvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(tn);
    tn_elm->append_text(tnvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(fn);
    fn_elm->append_text(fnvalue.str());

    stats_elm->append_text("\n      ");
  }

  bxml_write(fname, doc);

  return true;
}


void vox_mcs_eval::
read_cams()
{
  if (params_->cam_type_() == "intrinsic_extrinsic") {
    cam_type_ = mw_util::MW_INTRINSIC_EXTRINSIC;
  } else {
    if (params_->cam_type_() == "projcamera")
      cam_type_ = mw_util::MW_3X4;
    else  {
      vcl_cerr << "Error: invalid camera type " << params_->cam_type_() << vcl_endl;
      exit(1);
    }
  }
}

//: Read the list of data per frame
void vox_mcs_eval::
read_frame_data()
{
  bool retval = 
    mw_data::read_frame_data_list_txt(params_->input_folder_(), &dpath_, 
        cam_type_);
  if (!retval)
    exit(1);
  vcl_cout << "Dpath:\n" << dpath_ << vcl_endl;
}

//: Read the ground truth.
void vox_mcs_eval::
read_gt()
{
  vcl_string fname = params_->gt_object_dir_() + params_->gt_object_name_();
  vsl_b_ifstream bfs_in(fname);
  if (!bfs_in) {
    vcl_cout << "Error: unable to open file " << fname << vcl_endl;
    exit(1);
  }
  vsl_b_read(bfs_in, gt_);
  // vcl_cout << gt_;
}

void vox_mcs_eval::
read_one_mcs_instance()
{
  bool retval = dbmcs_view_set::read_txt(
      params_->input_folder_() + vcl_string("/mcs_stereo_instances.txt"), 
      &all_instances_);
  MW_ASSERT("frames to match from file", retval, true);

  MW_ASSERT("instance id valid", 
      params_->instance_id_() < all_instances_.num_instances(), true);

  one_instance_.add_instance(all_instances_.instance(params_->instance_id_()));

  vcl_cout << "Running on instance[" <<  params_->instance_id_() << "]: " 
    << one_instance_ << vcl_endl;
}

void vox_mcs_eval::
read_param_lists()
{
  mw_util::parse_num_list(params_->distance_threshold_list_(), &distance_threshold_list_);
  mw_util::parse_num_list(params_->dtheta_threshold_list_(), &dtheta_threshold_list_);
  mw_util::parse_num_list(params_->min_samples_per_curve_frag_list_(), &min_samples_per_curve_frag_list_);
  prune_by_length_ = params_->prune_by_length_();
  mw_util::parse_num_list(params_->min_inliers_per_view_list_(), &min_inliers_per_view_list_);
  mw_util::parse_num_list(params_->min_total_inliers_list_(), &min_total_inliers_list_);
  vcl_sort(min_total_inliers_list_.begin(), min_total_inliers_list_.end());
  mw_util::parse_num_list(params_->min_first_to_second_best_ratio_list_(), &min_first_to_second_best_ratio_list_);
  mw_util::parse_num_list(params_->lonely_threshold_list_(), &lonely_threshold_list_);
  mw_util::parse_num_list(params_->min_epipolar_overlap_list_(), &min_epipolar_overlap_list_);
  mw_util::parse_num_list(params_->min_epiangle_list_(), &min_epiangle_list_);

  use_curvelets_ = params_->use_curvelets_();
  mw_util::parse_num_list(params_->min_num_inlier_edgels_per_curvelet_list_(), &min_num_inlier_edgels_per_curvelet_list_);
}

bool 
write_stats_with_params(
    const vcl_string &dataset_name, 
    const vcl_vector<dborl_exp_stat> &stats_at_threshold,
    vcl_vector<vcl_vector<vcl_string> >eval_params_names,
    vcl_vector<vcl_vector<vcl_string> >eval_params_values,
    vcl_string fname
    )
{
  vcl_cout<<"Writing evaluation results to " << fname << vcl_endl;

  if (stats_at_threshold.empty()) {
    vcl_cerr << "Error: no stats.\n";
    return false;
  }

  assert (eval_params_names.size() == eval_params_values.size());
  assert (eval_params_names.size() == stats_at_threshold.size());
 
  // Xml Tree: eval_results->dataset->stats

  // Create root element
  bxml_document  doc;
  bxml_data_sptr root     = new bxml_element("eval_results");
  bxml_element*  root_elm = dbxml_algos::cast_to_element(root,"eval_results");
  doc.set_root_element(root); 
  root_elm->append_text("\n   ");

  // Create query element
  bxml_data_sptr dataset     = new bxml_element("dataset");
  bxml_element*  dataset_elm = 
      dbxml_algos::cast_to_element(dataset,"dataset");
  root_elm->append_data(dataset);
  root_elm->append_text("\n");
  dataset_elm->set_attribute("name",dataset_name);
  dataset_elm->set_attribute("num_positive",
                             stats_at_threshold[0].positive_cnt_);
  dataset_elm->set_attribute("num_negative",
                             stats_at_threshold[0].negative_cnt_);
  dataset_elm->append_text("\n      ");
  
  // Loop over vector of bounding boxes
  
  for (unsigned i = 0; i < stats_at_threshold.size(); ++i) {
    bxml_data_sptr stats     = new bxml_element("stats");
    bxml_element*  stats_elm = dbxml_algos::
        cast_to_element(stats,"stats");

    assert (eval_params_names[i].size() == eval_params_values[i].size());

    unsigned num_params = eval_params_names[i].size();

    for (unsigned k=0; k < num_params; ++k)
      stats_elm->set_attribute(eval_params_names[i][k], eval_params_values[i][k]);

    if ( i > 0 )
    {
        dataset_elm->append_text("   ");
    }
    dataset_elm->append_data(stats);
    dataset_elm->append_text("\n   ");
    
    // Append coordinates
    bxml_data_sptr tp = new bxml_element("TP");
    bxml_data_sptr fp = new bxml_element("FP");
    bxml_data_sptr tn = new bxml_element("TN");
    bxml_data_sptr fn = new bxml_element("FN");
    
    bxml_element* tp_elm=dbxml_algos::cast_to_element(tp,"TP");
    bxml_element* fp_elm=dbxml_algos::cast_to_element(fp,"FP");
    bxml_element* tn_elm=dbxml_algos::cast_to_element(tn,"TN");
    bxml_element* fn_elm=dbxml_algos::cast_to_element(fn,"FN");
   
    vcl_stringstream tpvalue,fpvalue,tnvalue,fnvalue;
    
    tpvalue<<stats_at_threshold[i].TP_;
    fpvalue<<stats_at_threshold[i].FP_;
    tnvalue<<stats_at_threshold[i].TN_;
    fnvalue<<stats_at_threshold[i].FN_;

    stats_elm->append_text("\n         ");
    stats_elm->append_data(tp);
    tp_elm->append_text(tpvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(fp);
    fp_elm->append_text(fpvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(tn);
    tn_elm->append_text(tnvalue.str());

    stats_elm->append_text("\n         ");
    stats_elm->append_data(fn);
    fn_elm->append_text(fnvalue.str());

    stats_elm->append_text("\n      ");
  }

  bxml_write(fname, doc);

  return true;
}
