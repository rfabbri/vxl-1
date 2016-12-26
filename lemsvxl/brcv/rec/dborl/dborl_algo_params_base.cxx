//:
// \file
// \brief
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
//

#include "dborl_algo_params_base.h"

#include <vcl_map.h>

vcl_string dborl_algo_params_base::output_file_postfix()
{
  vcl_string out;

  //: make one pass and find all the group names to be parsed
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: now print each group
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    vcl_string group_name = iter->first;
    out = out + "_" + group_name;
    //: go over the param list to get each param from this group
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;
      vcl_string val = param_list_[i]->value_str(); 
      out = out + "_" + val;
    }
  }

  return out;
}

vcl_string dborl_algo_params_base::output_file_postfix(vcl_string replacement_algo_name)
{
  vcl_string out;

  //: make one pass and find all the group names to be parsed
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: now print each group
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    vcl_string group_name = iter->first;
    
    //int pos = group_name.find_first_of(algo_name_);
    //int pos2 = group_name.find_last_of(algo_name_);
    vcl_string::size_type pos = algo_name_.length();
    //vcl_string new_name = group_name.substr(pos, group_name.length()-algo_name_.length());
    vcl_string new_name = group_name.substr(pos);
    //vcl_string new_name = group_name;
    new_name = replacement_algo_name + new_name;

    out = out + "_" + new_name;
    //: go over the param list to get each param from this group
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;
      vcl_string val = param_list_[i]->value_str(); 
      out = out + "_" + val;
    }
  }

  return out;
}

