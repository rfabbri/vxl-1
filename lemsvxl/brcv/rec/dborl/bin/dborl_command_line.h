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

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <dborl/dborl_dataset_sptr.h>

// datalist should contain a list such that each item is <objectname>[.<ext>] (the extension will be stripped if exists)
// for each <objectname> in the dataset_list, copy the images from image to the output folder under object's folder
// assuming: <objectname>.<ext> 
// open the corresponding con file, assuming: <objectname>.con
// and create the ground truth xml file based on the category information read from the category_info_file (xml formatted)
bool prepare_dataset_folder_from_images_and_cons(vcl_string image_folder, 
                                                 vcl_string image_ext, 
                                                 vcl_string con_folder, 
                                                 vcl_string dataset_list, 
                                                 vcl_string category_info_file, 
                                                 vcl_string output_folder);

// datalist should contain a list such that each item is <objectname>[.<ext>] (the extension will be stripped if exists)
// for each <objectname> in the dataset_list, copy the files from input folder to the output folder under object's folder
// assuming: <objectname>.<ext> exists in the input folder
bool prepare_dataset_folder_from_exts(vcl_string input_folder, 
                                                 vcl_string ext, 
                                                 vcl_string dataset_list, 
                                                 vcl_string output_folder, vcl_string category);

//: assumes each object has a folder of its own, under which the image and groundtruth file resides
//  the convention for the groundtruth file is <object name>.xml
dborl_dataset_sptr load_image_dataset(vcl_string dataset_folder, vcl_string image_ext);
bool prepare_flat_index_of_image_dataset(vcl_string dataset_folder, vcl_string image_ext, vcl_string out_file);

bool prepare_flat_index_from_dataset_list(vcl_string dataset_list, vcl_string dataset_folder, vcl_string out_file, vcl_string category);

//: copy the files with the given extension from the input folder to the dataset output folder where each element in the list has its own folder
bool copy_files_to_object_folders(vcl_string input_folder, vcl_string ext, vcl_string dataset_list, vcl_string dataset_folder);

//: copy the files with the given extension from the input orl-style folder to the output orl-style folder (orl-style: each element in the list has its own folder)
bool copy_files_from_to_object_folders(vcl_string input_folder, vcl_string ext, vcl_string dataset_list, vcl_string dataset_folder);

//: prepare the input xml file for the model name given, keep all the other parameters same as the given input file, only change the model_name
bool prepare_model_input_param_file(vcl_string input_file_xml, vcl_string output_file_xml, vcl_string model_name);
bool prepare_model_inputs_param_file(vcl_string input_file_xml, vcl_string db_list, vcl_string outputs_folder);

//: prepare a file with a command in each line 
//  command's in each line contains one of the elements in the dataset list, e.g. ith one as follows:
//  command_initial + db_list[i] + command_suffix 
bool prepare_commands_file(vcl_string command_initial, vcl_string command_suffix, vcl_string db_list, vcl_string out_file);

bool prepare_ground_truth_from_pascal_annot_ver_one(vcl_string input_folder, vcl_string dataset_list, vcl_string output_folder, vcl_string category);

//: a method to rename groundtruth.xml files to <object name>.xml according to the new convention
bool rename_ground_truth_files(vcl_string input_folder, vcl_string dataset_list, vcl_string category);

#endif  //_DBORL_COMMAND_LINE_H
