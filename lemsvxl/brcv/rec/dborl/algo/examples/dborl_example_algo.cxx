//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dborl_example_algo.h"
#include "dborl_example_algo_params.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_evaluation.h>
#include <vcl_iostream.h>
#include <vul/vul_file.h>

//: read the files etc
bool dborl_example_algo::initialize()
{
  if (vul_file::exists(params_->filename_string_())) {
    //: read the list of names from the file, using a dborl_utility
    if (!parse_strings_from_file(params_->filename_string_(), names_))
      return false;

    vcl_cout << "read " << names_.size() << " names as follows\n";
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_example_algo::process()
{
  //: do something for each name
  for (unsigned i = 0; i < names_.size(); i++) {
    params_->a_double_ = params_->a_float_() + params_->another_float_();

    //: increment the status parameter
    params_->percent_completed = ((float)i/names_.size())*100.0f;
    params_->status_param_ = params_->status_param_() + 1;
    params_->print_status_xml();
  }

  //: do something else
  for (int i = 0; i < params_->an_int_(); i++) {
    params_->a_double_ = params_->a_float_() + params_->another_float_();

    //: increment the status parameter
    params_->percent_completed = ((float)i/params_->an_int_())*100.0f;
    vcl_cout << params_->percent_completed() << " ";
    params_->status_param_ = params_->status_param_() + 1;
    params_->print_status_xml();
  }

  params_->print_status_xml();
  return true;
}

bool dborl_example_algo::finalize()
{
  params_->percent_completed = 100.0f;
  params_->print_status_xml();

  double tpr = params_->a_float_()/10.0f;
  double fpr = params_->another_float_()/10.0f;
  params_->perf_map_insert("some_category", fpr, tpr);
  params_->perf_plot_set_type(dborl_evaluation_plot_type::ROC);
  params_->print_perf_xml("ROC plot: TPR vs FPR");

  vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;
  dborl_exp_stat_sptr stat = new dborl_exp_stat();
  stat->increment_TP();
  stat->increment_FP();
  category_statistics["some other category"] = stat;

  params_->print_evaluation_xml(category_statistics, true);

  return true;
}

