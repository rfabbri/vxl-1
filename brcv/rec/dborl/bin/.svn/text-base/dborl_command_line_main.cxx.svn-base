//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/07
#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

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

#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>

#include "dborl_command_line.h"

int main(int argc, char *argv[]) {

  // Arguments
  vul_arg_info_list arg_list;
  //vul_arg<double> sigma(arg_list,"-sigma","sigma for Gaussian smoothing",1.0);
  //vul_arg<float> thresh1(arg_list,"-t1","Threshold for truncating motion",0.001f);
  //vul_arg<float> thresh2(arg_list,"-t2","Threshold for detecting motion",0.0001f);
  
  vul_arg<bool> prepare_dataset_folder(arg_list,"-prepare-dataset-folder","run the routine to prepare the dataset folder",false);
  vul_arg<bool> prepare_dataset_folder_from_exts_arg(arg_list,"-prepare-dataset-folder-exts","run the routine to prepare the dataset folder",false);
  vul_arg<bool> prepare_flat_index_of_dataset(arg_list,"-prepare-flat-index","run the routine to prepare the flat index of the dataset that resides in the input folder",false);
  vul_arg<vcl_string> input_folder(arg_list,"-input","the input folder","");
  vul_arg<vcl_string> output_file(arg_list,"-out","the output file name","");

  vul_arg<bool> copy_files_to_object_folders_arg(arg_list, "-copy-files-to-object-folders", "run the routine to copy files to dataset folder appropriately", false);
  vul_arg<bool> copy_files_from_to_object_folders_arg(arg_list, "-copy-files-from-to-object-folders", "run the routine to copy files from orl-style folder to another orl-style folder", false);

  vul_arg<bool> prepare_model_input_xml_for_detection(arg_list,"-prepare-model-input","run the routine to prepare a input xml file for the given model by reading all other params from the input xml file",false);
  vul_arg<vcl_string> input_xml(arg_list, "-inp-xml", "the input xml file to read all parameters", "");
  vul_arg<vcl_string> output_xml(arg_list, "-out-xml", "the name of the output xml file to write new model name and path", "");
  vul_arg<vcl_string> model_name(arg_list, "-model", "the name of the model to replace in the input xml file and write to output xml file", "");
  vul_arg<bool> prepare_model_inputs_xml_for_detection(arg_list,"-prepare-model-inputs","run the routine to prepare an input xml file for each model given by the dataset-list",false);

  vul_arg<bool> prepare_command(arg_list, "-prepare-command", "run the routine to prepare a file with a command in each line prepared for each element in the given dataset-list file", false);
  vul_arg<vcl_string> command_initial(arg_list, "-command-init", "the initial portion of the command, same for all elements", "");
  vul_arg<vcl_string> command_suffix(arg_list, "-command-suf", "the end portion of the command, will be appended after each elements name, may be empty", "");

  vul_arg<vcl_string> image_folder(arg_list,"-images","the folder that contains images","");
  vul_arg<vcl_string> image_ext(arg_list,"-ext","all the images in the folder with this extension are used","");
  
  vul_arg<vcl_string> dataset_list(arg_list,"-dataset-list","the file that contains the list of the elements in the dataset","");
  vul_arg<vcl_string> category_info_file(arg_list,"-category-info","the file that contains the info about the categories like names and prefixes","");
  
  vul_arg<vcl_string> output_folder(arg_list,"-output","the output folder of the routine","");

  vul_arg<vcl_string> con_folder(arg_list,"-cons","the folder that contains cons","");

  vul_arg<vcl_string> esf_folder(arg_list,"-esfs","the folder that contains esfs","");
  
  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);

  vul_arg<bool> prepare_ground_truth_from_pascal_annot_ver_one_arg(arg_list, "-prepare-gt-pascal-annot-ver-one", false);
  vul_arg<vcl_string> category(arg_list, "-category", "category name", "");

  vul_arg<bool> prepare_flat_index_from_dataset_list_arg(arg_list, "-prepare-flat-index-from-dataset-list", false);

  vul_arg<bool> rename_ground_truth_files_arg(arg_list, "-rename-gts", false);

  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  if (prepare_dataset_folder()) {
    vcl_cout << "image_folder: " << image_folder() << vcl_endl;
    vcl_cout << "image ext: " << image_ext() << vcl_endl;
    vcl_cout << "con folder: " << con_folder() << vcl_endl;
    vcl_cout << "dataset list: " << dataset_list() << vcl_endl;
    vcl_cout << "category_info_file: " << category_info_file() << vcl_endl;
    vcl_cout << "output_folder: " << output_folder() << vcl_endl;
    prepare_dataset_folder_from_images_and_cons(image_folder(), image_ext(), con_folder(), dataset_list(), category_info_file(), output_folder());
  } else if (prepare_dataset_folder_from_exts_arg()) {
    if (!prepare_dataset_folder_from_exts(input_folder(), image_ext(), dataset_list(), output_folder(), category())) {
      vcl_cout << "usage with -prepare-dataset-folder-exts: \n";
      vcl_cout << input_folder.option() << " " << input_folder.help() << vcl_endl;
      vcl_cout << image_ext.option() << " " << image_ext.help() << vcl_endl;
      vcl_cout << dataset_list.option() << " " << dataset_list.help() << vcl_endl;
      vcl_cout << output_folder.option() << " " << output_folder.help() << vcl_endl;
      vcl_cout << " if want to append category names to the beginning of the object names: " << category.option() << " " << category.help() << vcl_endl;
    }
  } else if (prepare_flat_index_of_dataset()) {
    vcl_cout << "input folder: " << input_folder() << vcl_endl;
    vcl_cout << "output index file name: " << output_file() << vcl_endl;
    if (image_ext().compare("") != 0 && input_folder().compare("") != 0 && output_file().compare("") != 0) {
      vcl_cout << "will prepare flat index of an image dataset with image extensions: " << image_ext() << vcl_endl;
      if (!prepare_flat_index_of_image_dataset(input_folder(), image_ext(), output_file()))
        vcl_cout << "usage with -prepare-flat-index: -ext <extension> -input <input dataset folder> -out <output index file>\n";
    } else 
      vcl_cout << "usage with -prepare-flat-index: -ext <extension> -input <input dataset folder> -out <output index file>\n";
  } else if (copy_files_to_object_folders_arg()) {
      if (!copy_files_to_object_folders(input_folder(), image_ext(), dataset_list(), output_folder()))
        vcl_cout << "usage with -copy-files-to-object-folders: -input <input folder> -ext <file extension> -dataset-list <dataset list> -output <output dataset folder>\n";
  } else if (copy_files_from_to_object_folders_arg()) {
      if (!copy_files_from_to_object_folders(input_folder(), image_ext(), dataset_list(), output_folder()))
        vcl_cout << "usage with -copy-files-from-to-object-folders: -input <input folder> -ext <file extension> -dataset-list <dataset list> -output <output dataset folder>\n";
  } else if (prepare_model_input_xml_for_detection()) {
      if (!prepare_model_input_param_file(input_xml(), output_xml(), model_name()))
        vcl_cout << "usage with -prepare-model-input: -inp-xml <input xml> -out-xml <output xml> -model <name of the model>\n";
  } else if (prepare_model_inputs_xml_for_detection()) {
      if (!prepare_model_inputs_param_file(input_xml(), dataset_list(), output_folder()))
        vcl_cout << "usage with -prepare-model-inputs: -inp-xml <input xml> -dataset-list <list of model names> -output <output folder to save output xml files>\n";
  } else if (prepare_command()) {
    if (!prepare_commands_file(command_initial(), command_suffix(), dataset_list(), output_file())) {
      vcl_cout << "usage with -prepare-command: \n";
      vcl_cout << command_initial.option() << " " << command_initial.help() << vcl_endl;
      vcl_cout << command_suffix.option() << " " << command_suffix.help()  << vcl_endl;
      vcl_cout << dataset_list.option() << " " << dataset_list.help() << vcl_endl;
      vcl_cout << output_file.option() << " " << output_file.help() << vcl_endl;
      vcl_cout << "\n";
    }
  } else if (prepare_ground_truth_from_pascal_annot_ver_one_arg()) {
    if (!prepare_ground_truth_from_pascal_annot_ver_one(input_folder(), dataset_list(), output_folder(), category())) {
      vcl_cout << "usage with -prepare-gt-pascal-annot-ver-one: \n";
      vcl_cout << input_folder.option() << " " << input_folder.help() << vcl_endl;
      vcl_cout << category.option() << " " << category.help() << vcl_endl;
      vcl_cout << dataset_list.option() << " " << dataset_list.help() << vcl_endl;
      vcl_cout << output_folder.option() << " " << output_folder.help() << vcl_endl;
    }
  } else if (prepare_flat_index_from_dataset_list_arg()) {
    if (!prepare_flat_index_from_dataset_list(dataset_list(), input_folder(), output_file(), category())) {
      vcl_cout << "usage with -prepare-flat-index-from-dataset-list: \n";
      vcl_cout << dataset_list.option() << " " << dataset_list.help() << vcl_endl;
      vcl_cout << input_folder.option() << " " << input_folder.help() << vcl_endl;
      vcl_cout << output_file.option() << " " << output_file.help() << vcl_endl;
      vcl_cout << category.option() << " " << category.help() << " if category name is to be appended to the object names in the list " << vcl_endl;
    }
  } else if (rename_ground_truth_files_arg()) {
    if (!rename_ground_truth_files(input_folder(), dataset_list(), category())) {
      vcl_cout << "usage with -rename-gts: \n";
      vcl_cout << dataset_list.option() << " " << dataset_list.help() << vcl_endl;
      vcl_cout << input_folder.option() << " " << input_folder.help() << vcl_endl;
      vcl_cout << "if -category <category name> option exists, then it appends the <category name> before the object name in the list\n";
    }
  }
  return 0;
}

