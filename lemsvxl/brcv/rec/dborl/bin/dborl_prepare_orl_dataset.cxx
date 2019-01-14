// classify shock patch sets

#include "dborl_command_line.h"
#include <dborl/algo/dborl_utilities.h>
#include <borld/borld_category_info_sptr.h>
#include <borld/borld_category_info.h>
#include <dborl/dborl_dataset.h>
#include <dborl/algo/dborl_category_info_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <borld/borld_image_bbox_description.h>
#include <borld/borld_image_description.h>
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


//: copy the files with the given extension from the input folder to the dataset output folder where each element in the list has its own folder
bool copy_files_to_object_folders(std::string input_folder, std::string ext, std::string dataset_list, std::string dataset_folder)
{
  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) 
    return false;

  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = vul_file::strip_extension(object_names[i]);  // if there is an extension at the end, just strip it
    
    std::string file = input_folder + object_name + "." + ext;

    if (!vul_file::exists(file.c_str())) {
      std::cout << "cannot find: " << file << "\n";
      return false;
    }

    std::string object_output_dir = dataset_folder + object_name + "/";

    std::string command = "cp ";
    command = command + file + " " + object_output_dir + object_name + "." + ext;
    std::cout << "\tcommand: " << command << std::endl;
    system(command.c_str());
  }

  return true;
}

//: copy the files with the given extension from the input orl-style folder to the output orl-style folder (orl-style: each element in the list has its own folder)
bool copy_files_from_to_object_folders(std::string input_folder, std::string ext, std::string dataset_list, std::string dataset_folder)
{
  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) 
    return false;

  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = vul_file::strip_extension(object_names[i]);  // if there is an extension at the end, just strip it
    
    std::string file = input_folder + "/" + object_name + "/" + object_name + ext;

    if (!vul_file::exists(file.c_str())) {
      std::cout << "cannot find: " << file << "\n";
      return false;
    }

    std::string object_output_dir = dataset_folder + object_name + "/";

    std::string command = "cp ";
    command = command + file + " " + object_output_dir + object_name + ext;
    std::cout << "\tcommand: " << command << std::endl;
    system(command.c_str());
  }

  return true;
}

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
                                                 std::string output_folder)
{   
  if (!vul_file::is_directory(output_folder)) {
    std::cout << "creating " << output_folder << "..\n";
    vul_file::make_directory(output_folder);
  }

  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) 
    return false;

  // read the category_info file
  std::vector<borld_category_info_sptr> categories;
  dborl_category_info_parser parser;
  if (!parse(category_info_file, parser, categories)) 
    return false;
  
  std::vector<std::string> object_categories;
  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = vul_file::strip_extension(object_names[i]);  // if there is an extension at the end, just strip it
    int cat_id = dborl_get_category(object_name, categories);
    if (cat_id < 0 || cat_id >= int(categories.size())) {
      std::cout << "object category not determined for: " << object_name << " in the category info file!! Exiting\n" << std::endl;
      return false;
    }
    object_categories.push_back(categories[cat_id]->name_);
    std::cout << "object: " << object_name << " from " << categories[cat_id]->name_ << std::endl;
  }
  
  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = vul_file::strip_extension(object_names[i]);  // if there is an extension at the end, just strip it
    std::string object_cat = object_categories[i];
    
    std::string image_file = image_folder + object_name + "." + image_ext;
    std::string con_file = con_folder + object_name + ".con";

    std::string object_output_dir = output_folder + object_name + "/";
    if (!vul_file::is_directory(object_output_dir)) {
      std::cout << "creating " << object_output_dir << "..\n";
      vul_file::make_directory(object_output_dir);
    }

    std::string command = "cp ";
    command = command + image_file + " " + object_output_dir + object_name + "." + image_ext;
    std::cout << "\tcommand: " << command << std::endl;
    system(command.c_str());

    //std::string output_xml = object_output_dir + "groundtruth.xml";
    std::string output_xml = object_output_dir + object_name + ".xml";
  
    if (!read_con_write_image_description_xml(con_file, object_cat, output_xml)) {
      std::cout << "problems in reading: " << con_file << "\n or writing: " << output_xml << std::endl;
      return false;
    }

  }

  return true;
}

// datalist should contain a list such that each item is <objectname>[.<ext>] (the extension will be stripped if exists)
// for each <objectname> in the dataset_list, copy the files from input folder to the output folder under object's folder
// assuming: <objectname>.<ext> exists in the input folder
bool prepare_dataset_folder_from_exts(std::string input_folder, 
                                                 std::string ext, 
                                                 std::string dataset_list, 
                                                 std::string output_folder,std::string category)
{   
  if (input_folder.compare("") == 0)
    return false;

  if (ext.compare("") == 0)
    return false;

  if (dataset_list.compare("") == 0)
    return false;

  if (output_folder.compare("") == 0)
    return false;

  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) 
    return false;

  bool append_category = false;
  if (category.compare("") != 0)
    append_category = true;

  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = object_names[i];  
    
    std::string ext_file = input_folder + object_name + ext;

    if (append_category)
      object_name = category + "_" + object_name;

    std::string object_output_dir = output_folder + object_name + "/";
    if (!vul_file::is_directory(object_output_dir)) {
      std::cout << "creating " << object_output_dir << "..\n";
      vul_file::make_directory(object_output_dir);
    }

    std::string command = "cp ";
    command = command + ext_file + " " + object_output_dir + object_name + ext;
    std::cout << "\tcommand: " << command << std::endl;
    system(command.c_str());
  }

  return true;
}

bool prepare_ground_truth_from_pascal_annot_ver_one(std::string input_folder, std::string dataset_list, std::string output_folder, std::string category)
{
  if (input_folder.compare("") == 0)
    return false;

  if (dataset_list.compare("") == 0)
    return false;

  if (output_folder.compare("") == 0)
    return false;

  if (category.compare("") == 0)
    return false;

  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) 
    return false;
  std::cout << "number of objects: " << object_names.size() << std::endl;
  std::cout.flush();

  for (unsigned i = 0; i < object_names.size(); i++) {
    std::string object_name = object_names[i];  
    //std::string output_fname = output_folder + object_name + "/groundtruth.xml";
    //std::string output_fname = output_folder + category + "_" + object_name + "/groundtruth.xml";
    std::string output_fname = output_folder + category + "_" + object_name + "/" + object_name + ".xml";
    
    if (category.compare("bg") == 0) {

      borld_image_bbox_description_sptr ip = new borld_image_bbox_description();
      vsol_box_2d_sptr dummy = new vsol_box_2d();
      dummy->add_point(0, 0);
      dummy->add_point(0, 1);
      ip->add_box(category, dummy);
      borld_image_description_sptr id = new borld_image_description(ip);
    
      std::ofstream os;
      os.open(output_fname.c_str(), std::ios::out);
      id->write_xml(os);
      os.close();

    } else {
     // std::string inp_file = input_folder + object_name + ".txt";
      std::string inp_file = input_folder + object_name + "_" + category + ".groundtruth";

      std::ifstream fp(inp_file.c_str());
      if (!fp) {
        std::cout<<" Unable to Open "<< inp_file <<std::endl;
        return 0;
      } else {
        std::cout<<" Reading "<< inp_file <<std::endl;
      }
//      char buffer[2000];
      float min_x, min_y, max_x, max_y;
/*
      //2)Read in file header.
      fp.getline(buffer,2000); //# PASCAL Annotation Version 1.00
      fp.getline(buffer,2000); //
      fp.getline(buffer,2000); //Image size (X x Y x C) : 0 x 0 x 0
      fp.getline(buffer,2000); //Database : "The Oxford/Graz data extension"
      fp.getline(buffer,2000); //Objects with ground truth : 1 { "PASHorse" }
      fp.getline(buffer,2000); //
      fp.getline(buffer,2000); //# Note that there might be other objects in the image
      fp.getline(buffer,2000); //# for which ground truth data has not been provided.
      fp.getline(buffer,2000); //
      fp.getline(buffer,2000); //# Top left pixel co-ordinates : (1, 1)
      fp.getline(buffer,2000); //
      fp.getline(buffer,2000); //# Details for object 1 ("PASHorse")
      fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
      fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
      std::cout << " read: " << buffer << std::endl;
      
      //Bounding box for object 1 "PASHorse" (Xmin, Ymin) - (Xmax, Ymax) : (5, 7) - (231, 170)
      fp.getline(buffer,2000);
  std::string dum;
  std::istringstream oss(std::string(buffer).c_str());
  
  char c;
  oss >> dum;  //b
  std::cout << dum << std::endl;
  oss >> dum;  //b
  std::cout << dum << std::endl;
  oss >> dum;  //f
  std::cout << dum << std::endl;
  oss >> dum;  // ob
  std::cout << dum << std::endl;
  oss >> dum;  // 1 
  std::cout << dum << std::endl;
  oss >> dum;  // "
  std::cout << dum << std::endl;
  oss >> dum; //(
  std::cout << dum << std::endl;
  oss >> dum; // Y
  std::cout << dum << std::endl;
  oss >> dum; // -
  std::cout << dum << std::endl;
  oss >> dum; // (
  std::cout << dum << std::endl;
  oss >> dum; // Y
  std::cout << dum << std::endl;
  oss >> dum; // :
  std::cout << dum << std::endl;
  oss >> c;
  std::cout << "c: " << c << std::endl;
  oss >> min_x;
  std::cout << "min_x: " << min_x << std::endl;
  oss >> c;
  std::cout << "c: " << c << std::endl;
  oss >> min_y;
  std::cout << "min_y: " << min_y << std::endl;
  oss >> c;
  std::cout << "c: " << c << std::endl;
  oss >> dum;
  std::cout << "dum: " << dum << std::endl;
  oss >> c;
  std::cout << "c: " << c << std::endl;
  oss >> max_x;
  std::cout << "max_x: " << max_x << std::endl;
  oss >> c;
  std::cout << "c: " << c << std::endl;
  oss >> max_y;
  std::cout << "max_y: " << max_y << std::endl;
*/

      borld_image_bbox_description_sptr ip = new borld_image_bbox_description();
    fp >> min_x;
    while (!fp.eof()) {
    fp >> min_y;
    fp >> max_x;
    fp >> max_y;

  std::cout << "read minx: " << min_x << " miny: " << min_y << " max_x: " << max_x << " max_y " << max_y << std::endl;

      vsol_box_2d_sptr b = new vsol_box_2d();
      b->add_point(min_x, min_y);
      b->add_point(max_x, max_y);
      ip->add_box(category, b);
      fp >> min_x;
    }
    fp.close();

      borld_image_description_sptr id = new borld_image_description(ip);
    
      std::ofstream os;
      os.open(output_fname.c_str(), std::ios::out);
      id->write_xml(os);
      os.close();
    }
  }
  return true;
}


/*
// NO NEED FOR A dataset-list file
// for each .con file in the con_folder, copy the images from image to the output folder under object's folder 
// and create the ground truth xml file based on the category information read from the category_info_file (xml formatted)
bool prepare_dataset_folder_from_images_and_cons(std::string image_folder, 
                                                 std::string image_ext, 
                                                 std::string con_folder, 
                                                 std::string category_info_file, 
                                                 std::string output_folder)
{
  if (!vul_file::is_directory(image_folder)) {
    std::cout << image_folder << " is not a directory\n";
  //  return false;
  }

  if (!vul_file::is_directory(con_folder)) {
    std::cout << con_folder << " is not a directory\n";
    //return false;
  }

  if (!vul_file::is_directory(output_folder)) {
    std::cout << "creating " << output_folder << "..\n";
    //vul_file::make_directory(output_folder);
  }

  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) {
    std::cout << "could not parse the file: " << dataset_list << std::endl;
    return false;
  }

  std::string file_set = con_folder + "*.con";
  std::cout << "iterating over the files: " << file_set << std::endl;
  for (vul_file_iterator fi(file_set); fi; ++fi)
  {
    if (!vul_file::exists(fi()))
      continue;
    
    // find the object name!
    std::string object_name_with_dir = vul_file::strip_extension(fi());
    std::string object_name = vul_file::strip_directory(object_name_with_dir);
    
    std::cout << "creating object: " << object_name << std::endl;
    
    // check if object_name.* exists in the image_folder
    std::string image_file = image_folder + object_name + "." + image_ext;
    if (!vul_file::exists(image_file)) {
      std::cout << "the image file: " << image_file << " could not be found!! will continue with the next, nothing will be done for this object!!\n";
      continue;
    }
    
    std::string object_output_dir = output_folder + object_name + "/";

    std::string command = "cp ";
    command = command + fi() + " " + object_output_dir + object_name + "." + image_ext;
    std::cout << "\tcommand: " << command;
  }
  

  return true;
}
*/

//: assumes each object has a folder of its own, under which the image and groundtruth file resides
// the convention for the name of the groundtruth file is: <object name>.xml
dborl_dataset_sptr load_image_dataset(std::string dataset_folder, std::string image_ext)
{
  dborl_dataset_sptr ds = new dborl_dataset();

  std::cout << "iterating over the object folders in: " << dataset_folder << std::endl;
  for (vul_file_iterator fi(dataset_folder + "/*"); fi; ++fi)
  {
    std::cout << "folder: " << fi() << std::endl;
    if (!vul_file::exists(fi()))
      continue;
    
    if (!vul_file::is_directory(fi()))
      continue;
    
    // find the object name!
    std::string object_name = vul_file::strip_directory(fi());
    
    std::cout << "creating object: " << object_name << std::endl;
    
    // check if object_name.* exists in the image_folder
    std::string image_file = fi();
    image_file = image_file + "/" + object_name + "." + image_ext;
    if (!vul_file::exists(image_file)) {
      std::cout << "the image file: " << image_file << " could not be found!! will continue with the next, nothing will be done for this object!!\n";
      continue;
    }
    
    vil_image_resource_sptr img = vil_load_image_resource(image_file.c_str());
    dborl_image_object_sptr io = new dborl_image_object(object_name, img);

    std::string groundtruth_file = fi();
    //groundtruth_file = groundtruth_file + "/groundtruth.xml";
    groundtruth_file = groundtruth_file + "/" + object_name + ".xml";
    dborl_image_desc_parser parser;
    borld_image_description_sptr id = borld_image_description_parse(groundtruth_file, parser);
    io->set_description(id);
    ds->add_object(io->cast_to_object_base());
  }
  
  return ds;
}

bool prepare_flat_index_of_image_dataset(std::string dataset_folder, std::string image_ext, std::string out_file)
{
  dborl_dataset_sptr ds = load_image_dataset(dataset_folder, image_ext);
  if (!ds)
    return false;

  ds->name_ = "99-db";
  std::cout << "loaded " << ds->size() << " objects into the dataset instance " << ds->name_ << " \n";

  std::map<std::string, int> cnts;
  ds->get_category_cnts(cnts);
  for (std::map<std::string, int>::iterator it = cnts.begin(); it != cnts.end(); it++)
    std::cout << "cat: " << it->first << " cnt: " << it->second << std::endl;

  dborl_index_sptr ind = ds->create_flat_index(dataset_folder + "/");

  //: write index
  std::ofstream os;
  os.open(out_file.c_str(), std::ios::out);
  ind->write_xml(os);
  os.close();

  return true;
}

bool prepare_flat_index_from_dataset_list(std::string dataset_list, std::string dataset_folder, std::string out_file, std::string category)
{
  if (dataset_list.compare("") == 0)
    return false;

  if (out_file.compare("") == 0)
    return false;

  if (dataset_folder.compare("") == 0)
    return false;

  bool append_cat_name = false;
  if (category.compare("") != 0)
    append_cat_name = true;

  std::vector<std::string> object_names;
  if (!parse_strings_from_file(dataset_list, object_names)) {
    std::cout << "could not parse the file: " << dataset_list << std::endl;
    return false;
  }

  dborl_index_node_sptr n = new dborl_index_node("root");
  for (unsigned i = 0; i < object_names.size(); i++) {
    if (append_cat_name) {
      n->add_name(category + "_" + object_names[i]);
      n->add_path(dataset_folder + category + "_" + object_names[i] + "/");
    } else {
      n->add_name(object_names[i]);
      n->add_path(dataset_folder + object_names[i] + "/");
    }
  }

  dborl_index_sptr ind = new dborl_index("flat_index");
  ind->add_root(n->cast_to_index_node_base());

  //: write index
  std::ofstream os;
  os.open(out_file.c_str(), std::ios::out);
  ind->write_xml(os);
  os.close();
  return true;
}

//: prepare the input xml file for the model name given, keep all the other parameters same as the given input file, only change the model_name
bool prepare_model_input_param_file(std::string input_file_xml, std::string output_file_xml, std::string model_name)
{
  if (input_file_xml.compare("") == 0)
    return false;

  if (output_file_xml.compare("") == 0)
    return false;

  if (model_name.compare("") == 0)
    return false;

  bxml_document param_doc = bxml_read(input_file_xml);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    std::cout << "params root is not ELEMENT\n";
    return false;
  }

  dbskr_detect_patch_params params;
  if (!params.parse_from_data(param_doc.root_element()))
    return false;

  params.model_dir_ = params.model_dir_.substr(0, (params.model_dir_.length()-(params.model_name_.length()+1)));
  params.model_dir_ = params.model_dir_ + model_name + "/";
  params.model_name_ = model_name;


  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_detect_patch_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params.create_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(output_file_xml, doc);
  return true;
}

bool prepare_model_inputs_param_file(std::string input_file_xml, std::string db_list, std::string outputs_folder)
{
  if (input_file_xml.compare("") == 0)
    return false;

  if (db_list.compare("") == 0)
    return false;

  if (outputs_folder.compare("") == 0)
    return false;

  std::vector<std::string> db_names;
  if (!parse_strings_from_file(db_list, db_names))
    return false;
  if (!db_names.size()) {
    std::cout << "db list is empty or parsing problems!\n";
    return false;
  }

  for (unsigned i = 0; i < db_names.size(); i++) {
    std::string out_xml_name = outputs_folder + "inp-" + db_names[i] + ".xml";
    if (!prepare_model_input_param_file(input_file_xml, out_xml_name, db_names[i]))
      return false;
  }

  return true;
}


//: prepare a file with a command in each line 
//  command's in each line contains one of the elements in the dataset list, e.g. ith one as follows:
//  command_initial + db_list[i] + command_suffix 
bool prepare_commands_file(std::string command_initial, std::string command_suffix, std::string db_list, std::string out_file)
{
  if (command_initial.compare("") == 0)
    return false;

  if (db_list.compare("") == 0)
    return false;

  if (out_file.compare("") == 0)
    return false;

  std::vector<std::string> db_names;
  if (!parse_strings_from_file(db_list, db_names))
    return false;
  if (!db_names.size()) {
    std::cout << "db list is empty or parsing problems!\n";
    return false;
  }

  std::ofstream of(out_file.c_str());
  if (!of)
    return false;

  for (unsigned i = 0; i < db_names.size(); i++) 
   of << command_initial + db_names[i] + command_suffix << std::endl;

  return true;
}

//: a method to rename groundtruth.xml files to <object name>.xml according to the new convention
bool rename_ground_truth_files(std::string input_folder, std::string dataset_list, std::string category)
{
  if (dataset_list.compare("") == 0)
    return false;

  if (input_folder.compare("") == 0)
    return false;

  bool append_category = false;
  if (category.compare("") != 0)
    append_category = true;

  std::vector<std::string> db_names;
  if (!parse_strings_from_file(dataset_list, db_names))
    return false;
 
  for (unsigned i = 0; i < db_names.size(); i++) {
    std::string old_gt_file, new_gt_file;
    if (append_category) {
      old_gt_file = input_folder + category + "_" + db_names[i] + "/groundtruth.xml";
      new_gt_file = input_folder + category + "_" + db_names[i] + "/" + category + "_" + db_names[i] + ".xml";
    } else {
      old_gt_file = input_folder + db_names[i] + "/groundtruth.xml";
      new_gt_file = input_folder + db_names[i] + "/" + db_names[i] + ".xml";
    }

    std::string command = "mv ";
    command = command + old_gt_file + " " + new_gt_file;
    std::cout << "\tcommand: " << command << std::endl;
    system(command.c_str());
  }

  return true;
}

