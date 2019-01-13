// This is bbasd/buld/buld_parse_simple_file.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

//:
// \file

#include "buld_parse_simple_file.h"
//#include <cstdlib>

#include <iostream>
#include <fstream>
#include <vul/vul_string.h>
#include <vul/vul_awk.h>
#include <cstring>
#include <utility>

bool buld_parse_string_list(const std::string& input_file, 
                            std::vector<std::string >& string_list)
{
  string_list.clear();

  // parse image list
  std::ifstream ifs(input_file.c_str());
  if (!ifs) 
  {
    std::cout << "ERROR: Unable to open input file " << input_file << std::endl;
    return false;
  }

  // read the image names, one by one
  while (!ifs.eof()) 
  {
    // read the full line in
    std::string line;
    std::getline(ifs, line);
    
    // ignore lines starting with "#"
    if (line.empty() || line[0] == '#') 
      continue;

    std::string name = line.substr(0, line.find_first_of(' '));
    if (!name.empty())
      string_list.push_back(name);
  }
  ifs.close();
  return true;
}

bool 
buld_parse_number_lists (
      const std::string &fname,
      std::vector< std::vector<int> > &number_lists)
{
  std::ifstream infp(fname.c_str(), std::ios::in);
  if (!infp) {
    std::cerr << "buld: Error, unable to open file name " 
      << fname << std::endl;
    return false;
  }

  vul_awk awk(infp, vul_awk::strip_comments);

  for (; awk; ++awk) {
    if (!awk.NF())
      continue;
    std::vector<int> line_list(awk.NF());
    for (int i=0; i < awk.NF(); ++i) {
      // std::cout << "Line: " << awk.NR() << ", element[" << i << "] = " << awk[i] << std::endl;
      line_list[i] = vul_string_atoi(awk[i]);
    }
    number_lists.push_back(line_list);
  }

  return true;
}










//: Parse an ini-style parameter file
// For file format, check out: http://en.wikipedia.org/wiki/INI_file
// Example: 
// 
// ; comments
// [ccm_parameters]
// nbins_0to_pi=18
//
// ; another comment
// 
// [positive_data]
// perturb_delta_x=2
// perturb_delta_y=2
//
// Output: a map with  key  : section name
//                     value: list of key-value pairs in the section
bool buld_parse_ini_file(const std::string& fname,
                         std::map<std::string, std::map<std::string, std::string > >& section_params)
{
  section_params.clear();
  std::map<std::string, std::string >* cur_sec = 0;


  std::ifstream infp(fname.c_str(), std::ios::in);

  // Read in each line
  char line_buffer[1024];
  int line_number = 0;
  while (infp.getline(line_buffer, 1024)) 
  {
    ++line_number;

    //ignore comment lines and empty lines
    if (std::strlen(line_buffer)<2 || line_buffer[0]=='#' || line_buffer[0]==';')
      continue;

    std::string line(line_buffer);

    // begin a new section
    if (line[0] == '[') 
    {
      // fine the closing bracket
      std::string::size_type closing_bracket = line.find_first_of(']');
      if (closing_bracket == std::string::npos)
      {
        std::cout << "\nERROR: can't process line " << line_number << ".";
        return false;
      }

      std::string section_name = line.substr(1, closing_bracket-1);
      cur_sec = &(section_params[section_name]);
    }
    // parse value-pair
    else
    {
      // ignore everyting before the first section
      if (!cur_sec)
        continue;

      std::string::size_type equal_sign = line.find_first_of('=');
      if (equal_sign == std::string::npos)
      {
        std::cout << "\nERROR: can't process line " << line_number << ".";
        return false;
      }
      
      // key-value pair
      std::string key = line.substr(0, equal_sign);
      std::string value = line.substr(equal_sign+1, line.size()-equal_sign-1);

      if (key.empty())
        continue;

      (*cur_sec)[key] = value;
    }
  }
  return true;
}
