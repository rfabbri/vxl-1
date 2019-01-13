// This is bbasd/buld/cmdline/buld_embed_files_to_vxl_source_utils.h

#ifndef BULD_EMBED_FILES_TO_VXL_SOURCE_UTILS_H_
#define BULD_EMBED_FILES_TO_VXL_SOURCE_UTILS_H_

//:
// \file
// \brief command line utilities for shock-based segmentation
//
// \author Based on original code by  Firat Kalaycilar (@lems.brown.edu)
// \date Feb 09, 2010

#include<string>
#include<vector>

std::string getRandomVariableName(int len);

std::string getRandomHFileTag(int len);

void parseInputFile(std::string& input_file_name, std::vector<std::string>& original_files, std::vector<std::string>& variable_names, std::string& root_path);

int getArrayString(std::string filename, std::string& varname, std::string& array_string);

void create_h_file(std::string& filename, std::string& h_file_tag, std::string& extract_command, std::string& delete_command);

void create_cpp_file(std::string& output_cpp_file_name, std::string& output_h_file_name, std::string& root_path, std::vector<std::string>& original_files, std::vector<std::string>& variable_names, std::vector<int>& data_sizes, std::string& extract_command, std::string& delete_command);

#endif /* BULD_EMBED_FILES_TO_VXL_SOURCE_UTILS_H_ */
