// classify shock patch sets

#include "dborl_cmd_line.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_category_info_sptr.h>
#include <dborl/dborl_category_info.h>
#include <dborl/dborl_dataset.h>
#include <dborl/algo/dborl_category_info_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_object_sptr.h>
#include <dborl/dborl_image_object.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index_leaf_sptr.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <brip/brip_vil_float_ops.h>
#include <bxml/bxml_document.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/algo/io/dbskr_detect_patch_params.h>



//: also prepares the command file
bool prepare_param_files(vcl_string input_file_xml, vcl_string output_prefix, float min_val, float inc, float max_value)
{
  if (input_file_xml.compare("") == 0) {
    vcl_cout << "input_file_xml not specified\n";
    return false;
  }

  if (output_prefix.compare("") == 0) {
    vcl_cout << "output_prefix not specified\n";
    return false;
  }

  dborl_patch_match_params params;
  params.input_param_filename_ = input_file_xml;

  if (!params.parse_input_xml()) {
    vcl_cout << "could not parse file: " << params.input_param_filename_ << vcl_endl;
    return false;
  }

  vcl_ofstream of((output_prefix + "command.out").c_str());
  if (!of) {
    vcl_cout << "cannot open file: " << output_prefix + "command.out" << vcl_endl;
    return false;
  }
  vcl_string evaluation_file_base = params.evaluation_file();

  vcl_string command_initial = "mpirun -np 42 -machinefile /vision/projects/kimia/categorization/99-db/orl-exps/hosts -nolocal";
  command_initial = command_initial + " /share/apps/lemsvxlnew/release/brcv/rec/dborl/algo/mpi/shock/dborl_patch_match -x ";
  for (float thres = min_val; thres <= max_value; thres += inc) {
    vcl_stringstream ss;
    ss << thres;

    params.detection_params_.sim_threshold_ = thres;
    params.evaluation_file = evaluation_file_base + "_" + ss.str() + ".xml";
    
    vcl_string xml_name = output_prefix + ss.str() + ".xml";
    params.print_input_xml(xml_name);
    of << command_initial + xml_name << vcl_endl;
  }

  of.close();

  return true;
}

bool prepare_param_files2(vcl_string input_file_xml, vcl_string output_prefix, float min_val, float inc, float max_value, int processor_cnt)
{
  if (input_file_xml.compare("") == 0) {
    vcl_cout << "input_file_xml not specified\n";
    return false;
  }

  if (output_prefix.compare("") == 0) {
    vcl_cout << "output_prefix not specified\n";
    return false;
  }

  if (processor_cnt < 0) {
    vcl_cout << "np not specified\n";
    return false;
  }

  dborl_patch_match_params params;
  params.input_param_filename_ = input_file_xml;

  if (!params.parse_input_xml()) {
    vcl_cout << "could not parse file: " << params.input_param_filename_ << vcl_endl;
    return false;
  }

  vcl_ofstream of((output_prefix + "command.out").c_str());
  if (!of) {
    vcl_cout << "cannot open file: " << output_prefix + "command.out" << vcl_endl;
    return false;
  }
  vcl_string evaluation_file_base = params.evaluation_file();

  vcl_stringstream np;
  np << processor_cnt;

  vcl_string command_initial = "mpirun -np " + np.str() + " -machinefile /vision/projects/kimia/categorization/99-db/orl-exps/hosts -nolocal";
  command_initial = command_initial + " /share/apps/lemsvxlnew/release/brcv/rec/dborl/algo/mpi/shock/dborl_patch_match -x ";
  for (float thres = min_val; thres <= max_value; thres += inc) {
    vcl_stringstream ss;
    ss << thres;

    params.detection_params_.sim_threshold_ = thres;
    params.evaluation_file = evaluation_file_base + "_" + ss.str() + ".xml";
    
    vcl_string xml_name = output_prefix + ss.str() + ".xml";
    params.print_input_xml(xml_name);
    int total = 99*10;
    int times = (int)vcl_ceil((float)total/processor_cnt);
    vcl_cout << "total 990 and there are: " << processor_cnt << " processors so will run it: " << times << " times\n";
    for (int i = 0; i < times; i++)
      of << command_initial + xml_name << vcl_endl;
    
    params.compute_one_per_computer_ = false;
    vcl_string xml_name2 = output_prefix + ss.str() + "_final.xml";
    of << command_initial + xml_name2 << vcl_endl;
    params.print_input_xml(xml_name2);
  }

  of.close();

  return true;
}

bool prepare_plot(vcl_string input_file_prefix, vcl_string output_file, vcl_string legend_file, vcl_string plot_type, float min_val, float inc, float max_value)
{
  if (input_file_prefix.compare("") == 0) {
    vcl_cout << "input_file_xml not specified\n";
    return false;
  }

  if (output_file.compare("") == 0) {
    vcl_cout << "output_prefix not specified\n";
    return false;
  }

  if (legend_file.compare("") == 0) {
    vcl_cout << "legend_file not specified\n";
    return false;
  }

  if (plot_type.compare("") == 0) {
    vcl_cout << "plot_type not specified\n";
    return false;
  }

  vcl_vector<vcl_string> legends;
  if (!parse_lines_from_file(legend_file, legends))
    return false;

  vcl_vector<dborl_exp_stat_sptr> tmp;
  vcl_vector<vcl_vector<dborl_exp_stat_sptr> > legend_stats(legends.size(), tmp);

  //vcl_string input_files = input_file_prefix + "*.xml";
  //for (vul_file_iterator fi(input_files); fi; ++fi)
  for (float thres = min_val; thres <= max_value; thres += inc) {
    vcl_stringstream ss;
    ss << thres;

    vcl_string input_file = input_file_prefix + ss.str() + ".xml";
    vcl_cout << "will parse: " << input_file << vcl_endl;
    if (!vul_file::exists(input_file)) {
      vcl_cout << " does not exist!!\n";
      continue;
    }

    vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;
    vcl_string algo_name;
    
    if (!parse_evaluation_file(input_file, category_statistics, algo_name)) {
      vcl_cout << "problems in parsing: " << input_file << vcl_endl;
      return false;
    }
    vcl_cout << "parsed: " << input_file << vcl_endl;
    for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.begin(); it != category_statistics.end(); it++) {
      vcl_cout << it->first << " " << (it->second)->TP_ << " " << (it->second)->FP_ << " " << (it->second)->TN_ << " " << (it->second)->FN_ << vcl_endl;
    }

    //: find the stats of each legend
    for (unsigned i = 0; i < legends.size(); i++) {
      vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.find(legends[i]);
      if (it != category_statistics.end()) {
        //vcl_cout << "found: " << legends[i] << vcl_endl;
        it->second->update_positive_count();
        it->second->update_negative_count();  // makes sense in categorization where FP + TN actually equals N-
        legend_stats[i].push_back(it->second);
      }
    }

  }

  vcl_ofstream of(output_file.c_str());
  if (!of)
    return false;

  for (unsigned i = 0; i < legends.size(); i++) {
    of << legends[i] << vcl_endl;
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    if (plot_type.compare("roc") == 0) {
      dborl_evaluation_print_ROC_data(tmp, of);
      of << "ROC EER:\n" << dborl_evaluation_ROC_EER(tmp) << vcl_endl;
    } else if (plot_type.compare("prc") == 0) {
      dborl_evaluation_print_PRC_data(tmp, of);
      of << "PRC EER:\n" << dborl_evaluation_PRC_EER(tmp) << vcl_endl;
    } else if (plot_type.compare("rpc") == 0) {
      dborl_evaluation_print_RPC_data(tmp, of);
      of << "RPC EER:\n" << dborl_evaluation_RPC_EER(tmp) << vcl_endl;
    } else {
      vcl_cout << "unknown plot type!!\n";
      of.close();
      return false;
    }
    //vcl_cout << legends[i] << " " << tmp[0]->TP_ << " " << tmp[0]->FP_ << " " << tmp[0]->TN_ << " " << tmp[0]->FN_ << vcl_endl;
  }
  
  of.close();

  vcl_string no_ext = vul_file::strip_extension(output_file.c_str());

  vcl_ofstream of2((no_ext + ".m").c_str());
  if (!of2)
    return false;

  of2 << "figure;\n";  // plot ROC's first
  of2 << "set(gcf,'Color',[1,1,1]);\n";  // set figure background to white

  for (unsigned i = 0; i < legends.size(); i++) {
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    vcl_vector<float> xs, ys;
    dborl_evaluation_get_ROC_data(tmp, xs, ys);
    float eer = dborl_evaluation_ROC_EER(tmp);
    of2 << "x = [";
    for (unsigned j = 0; j < xs.size(); j++) {
      of2 << xs[j] << " ";
    }
    of2 << "];\n";
    of2 << "y = [";
    for (unsigned j = 0; j < ys.size(); j++) {
      of2 << ys[j] << " ";
    }
    of2 << "];\n";
    of2 << "subplot(1," << legends.size() << "," << i+1 << ");\n";   //    subplot(1,4,4)    
    of2 << "plot(x, y,'*-')\n";
    of2 << "xlabel('FPR')\n";
    of2 << "ylabel('TPR')\n";
    of2 << "title(['ROC " << legends[i] << " " << eer << "'])\n";
    of2 << "axis([-0.1 1.1 -0.1 1.1])\n";
  }

  of2 << "figure;\n";  // plot PRC's first
  of2 << "set(gcf,'Color',[1,1,1]);\n";  // set figure background to white

  for (unsigned i = 0; i < legends.size(); i++) {
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    vcl_vector<float> xs, ys;
    dborl_evaluation_get_PRC_data(tmp, xs, ys);
    float eer = dborl_evaluation_PRC_EER(tmp);

    of2 << "x = [";
    for (unsigned j = 0; j < xs.size(); j++) {
      of2 << xs[j] << " ";
    }
    of2 << "];\n";
    of2 << "y = [";
    for (unsigned j = 0; j < ys.size(); j++) {
      of2 << ys[j] << " ";
    }
    of2 << "];\n";
    of2 << "subplot(1," << legends.size() << "," << i+1 << ");\n";   //    subplot(1,4,4)    
    of2 << "plot(x, y,'*-')\n";
    of2 << "xlabel('recall')\n";
    of2 << "ylabel('precision')\n";
    of2 << "title(['PRC " << legends[i] << " " << eer << "'])\n";
    of2 << "axis([-0.1 1.1 -0.1 1.1])\n";
  }
  
  
  of2.close();

  return true;
}

bool prepare_plot2(vcl_string input_file_prefix, vcl_string input_file_prefix2, vcl_string output_file, vcl_string legend_file, vcl_string plot_type, float min_val, float inc, float max_value)
{
  if (input_file_prefix.compare("") == 0) {
    vcl_cout << "input_file_xml not specified\n";
    return false;
  }

  if (input_file_prefix2.compare("") == 0) {
    vcl_cout << "input_file_xml not specified\n";
    return false;
  }

  if (output_file.compare("") == 0) {
    vcl_cout << "output_prefix not specified\n";
    return false;
  }

  if (legend_file.compare("") == 0) {
    vcl_cout << "legend_file not specified\n";
    return false;
  }

  if (plot_type.compare("") == 0) {
    vcl_cout << "plot_type not specified\n";
    return false;
  }

  vcl_vector<vcl_string> legends;
  if (!parse_lines_from_file(legend_file, legends))
    return false;

  vcl_vector<dborl_exp_stat_sptr> tmp;
  vcl_vector<vcl_vector<dborl_exp_stat_sptr> > legend_stats(legends.size(), tmp);

  //vcl_string input_files = input_file_prefix + "*.xml";
  //for (vul_file_iterator fi(input_files); fi; ++fi)
  for (float thres = min_val; thres <= max_value; thres += inc) {
    vcl_stringstream ss;
    ss << thres;

    vcl_string input_file = input_file_prefix + ss.str() + ".xml";
    vcl_cout << "will parse: " << input_file << vcl_endl;
    if (!vul_file::exists(input_file)) {
      vcl_cout << " does not exist!!\n";
      continue;
    }

    vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;
    vcl_string algo_name;
    
    if (!parse_evaluation_file(input_file, category_statistics, algo_name)) {
      vcl_cout << "problems in parsing: " << input_file << vcl_endl;
      return false;
    }
    vcl_cout << "parsed: " << input_file << vcl_endl;
    for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.begin(); it != category_statistics.end(); it++) {
      vcl_cout << it->first << " " << (it->second)->TP_ << " " << (it->second)->FP_ << " " << (it->second)->TN_ << " " << (it->second)->FN_ << vcl_endl;
    }

    //: find the stats of each legend
    for (unsigned i = 0; i < legends.size(); i++) {
      vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.find(legends[i]);
      if (it != category_statistics.end()) {
        //vcl_cout << "found: " << legends[i] << vcl_endl;
        it->second->update_positive_count();
        it->second->update_negative_count();  // makes sense in categorization where FP + TN actually equals N-
        legend_stats[i].push_back(it->second);
      }
    }

  }

  unsigned cnt = 0;
  for (float thres = min_val; thres <= max_value; thres += inc, cnt++) {
    vcl_stringstream ss;
    ss << thres;

    vcl_string input_file2 = input_file_prefix2 + ss.str() + ".xml";
    vcl_cout << "will parse: " << input_file2 << vcl_endl;
    if (!vul_file::exists(input_file2)) {
      vcl_cout << " does not exist!!\n";
      continue;
    }

    vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;
    vcl_string algo_name;
    
    if (!parse_evaluation_file(input_file2, category_statistics, algo_name)) {
      vcl_cout << "problems in parsing: " << input_file2 << vcl_endl;
      return false;
    }
    vcl_cout << "parsed: " << input_file2 << vcl_endl;
    for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.begin(); it != category_statistics.end(); it++) {
      vcl_cout << it->first << " " << (it->second)->TP_ << " " << (it->second)->FP_ << " " << (it->second)->TN_ << " " << (it->second)->FN_ << vcl_endl;
    }

    //: find the stats of each legend
    for (unsigned i = 0; i < legends.size(); i++) {
      vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = category_statistics.find(legends[i]);
      if (it != category_statistics.end()) {
        //vcl_cout << "found: " << legends[i] << vcl_endl;
        legend_stats[i][cnt]->increment_TP_by(it->second->TP_);
        legend_stats[i][cnt]->increment_FP_by(it->second->FP_);
        legend_stats[i][cnt]->increment_TN_by(it->second->TN_);
        legend_stats[i][cnt]->increment_FN_by(it->second->FN_);
        legend_stats[i][cnt]->update_positive_count();
        legend_stats[i][cnt]->update_negative_count();
      }
    }

  }

  vcl_ofstream of(output_file.c_str());
  if (!of)
    return false;

  for (unsigned i = 0; i < legends.size(); i++) {
    of << legends[i] << vcl_endl;
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    if (plot_type.compare("roc") == 0) {
      dborl_evaluation_print_ROC_data(tmp, of);
      of << "ROC EER:\n" << dborl_evaluation_ROC_EER(tmp) << vcl_endl;
    } else if (plot_type.compare("prc") == 0) {
      dborl_evaluation_print_PRC_data(tmp, of);
      of << "PRC EER:\n" << dborl_evaluation_PRC_EER(tmp) << vcl_endl;
    } else if (plot_type.compare("rpc") == 0) {
      dborl_evaluation_print_RPC_data(tmp, of);
      of << "RPC EER:\n" << dborl_evaluation_RPC_EER(tmp) << vcl_endl;
    } else {
      vcl_cout << "unknown plot type!!\n";
      of.close();
      return false;
    }
    //vcl_cout << legends[i] << " " << tmp[0]->TP_ << " " << tmp[0]->FP_ << " " << tmp[0]->TN_ << " " << tmp[0]->FN_ << vcl_endl;
  }
  
  of.close();

  vcl_string no_ext = vul_file::strip_extension(output_file.c_str());

  vcl_ofstream of2((no_ext + ".m").c_str());
  if (!of2)
    return false;

  of2 << "figure;\n";  // plot ROC's first
  of2 << "set(gcf,'Color',[1,1,1]);\n";  // set figure background to white

  for (unsigned i = 0; i < legends.size(); i++) {
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    vcl_vector<float> xs, ys;
    dborl_evaluation_get_ROC_data(tmp, xs, ys);
    float eer = dborl_evaluation_ROC_EER(tmp);
    of2 << "x = [";
    for (unsigned j = 0; j < xs.size(); j++) {
      of2 << xs[j] << " ";
    }
    of2 << "];\n";
    of2 << "y = [";
    for (unsigned j = 0; j < ys.size(); j++) {
      of2 << ys[j] << " ";
    }
    of2 << "];\n";
    of2 << "subplot(1," << legends.size() << "," << i+1 << ");\n";   //    subplot(1,4,4)    
    of2 << "plot(x, y,'*-')\n";
    of2 << "xlabel('FPR')\n";
    of2 << "ylabel('TPR')\n";
    of2 << "title(['ROC " << legends[i] << " " << eer << "'])\n";
    of2 << "axis([-0.1 1.1 -0.1 1.1])\n";
  }

  of2 << "figure;\n";  // plot PRC's first
  of2 << "set(gcf,'Color',[1,1,1]);\n";  // set figure background to white

  for (unsigned i = 0; i < legends.size(); i++) {
    vcl_vector<dborl_exp_stat_sptr> tmp = legend_stats[i];
    if (!tmp.size()) {
      vcl_cout << "could not find " << legends[i] << " as a legend\n";
      continue;
    }
    vcl_vector<float> xs, ys;
    dborl_evaluation_get_PRC_data(tmp, xs, ys);
    float eer = dborl_evaluation_PRC_EER(tmp);

    of2 << "x = [";
    for (unsigned j = 0; j < xs.size(); j++) {
      of2 << xs[j] << " ";
    }
    of2 << "];\n";
    of2 << "y = [";
    for (unsigned j = 0; j < ys.size(); j++) {
      of2 << ys[j] << " ";
    }
    of2 << "];\n";
    of2 << "subplot(1," << legends.size() << "," << i+1 << ");\n";   //    subplot(1,4,4)    
    of2 << "plot(x, y,'*-')\n";
    of2 << "xlabel('recall')\n";
    of2 << "ylabel('precision')\n";
    of2 << "title(['PRC " << legends[i] << " " << eer << "'])\n";
    of2 << "axis([-0.1 1.1 -0.1 1.1])\n";
  }
  
  
  of2.close();

  return true;
}


