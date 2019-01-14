//:
// \file
// \brief command line utilities for ORL experiments
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBORL_COMMAND_LINE_H)
#define _DBORL_COMMAND_LINE_H

#include <ctime>
#include <algorithm>
#include <iostream>
#include <string>
#include <dborl/dborl_dataset_sptr.h>

// datalist should contain a list such that each item is <objectname>[.<ext>] (the extension will be stripped if exists)
// for each <objectname> in the dataset_list, copy the images from image to the output folder under object's folder
// assuming: <objectname>.<ext> 
// open the corresponding con file, assuming: <objectname>.con
// and create the ground truth xml file based on the category information read from the category_info_file (xml formatted)
bool prepare_dataset_folder_from_images_and_cons(std::string image_folder, 
                                                 std::string image_ext, 
                                                 std::string con_folder, 
                                                 std::string dataset_list, 
                                                 std::string category_info_file, 
                                                 std::string output_folder);

// datalist should contain a list such that each item is <objectname>[.<ext>] (the extension will be stripped if exists)
// for each <objectname> in the dataset_list, copy the files from input folder to the output folder under object's folder
// assuming: <objectname>.<ext> exists in the input folder
bool prepare_dataset_folder_from_exts(std::string input_folder, 
                                                 std::string ext, 
                                                 std::string dataset_list, 
                                                 std::string output_folder, std::string category);

//: assumes each object has a folder of its own, under which the image and groundtruth file resides
//  the convention for the groundtruth file is <object name>.xml
dborl_dataset_sptr load_image_dataset(std::string dataset_folder, std::string image_ext);
bool prepare_flat_index_of_image_dataset(std::string dataset_folder, std::string image_ext, std::string out_file);

bool prepare_flat_index_from_dataset_list(std::string dataset_list, std::string dataset_folder, std::string out_file, std::string category);

//: copy the files with the given extension from the input folder to the dataset output folder where each element in the list has its own folder
bool copy_files_to_object_folders(std::string input_folder, std::string ext, std::string dataset_list, std::string dataset_folder);

//: copy the files with the given extension from the input orl-style folder to the output orl-style folder (orl-style: each element in the list has its own folder)
bool copy_files_from_to_object_folders(std::string input_folder, std::string ext, std::string dataset_list, std::string dataset_folder);

//: prepare the input xml file for the model name given, keep all the other parameters same as the given input file, only change the model_name
bool prepare_model_input_param_file(std::string input_file_xml, std::string output_file_xml, std::string model_name);
bool prepare_model_inputs_param_file(std::string input_file_xml, std::string db_list, std::string outputs_folder);

//: prepare a file with a command in each line 
//  command's in each line contains one of the elements in the dataset list, e.g. ith one as follows:
//  command_initial + db_list[i] + command_suffix 
bool prepare_commands_file(std::string command_initial, std::string command_suffix, std::string db_list, std::string out_file);

bool prepare_ground_truth_from_pascal_annot_ver_one(std::string input_folder, std::string dataset_list, std::string output_folder, std::string category);

//: a method to rename groundtruth.xml files to <object name>.xml according to the new convention
bool rename_ground_truth_files(std::string input_folder, std::string dataset_list, std::string category);

#endif  //_DBORL_COMMAND_LINE_H
