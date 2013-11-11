//:
// \file
// \author Isabel Restrepo
// \date 6-Apr-2011

#include "bof_scene_categories.h"

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <boxm/boxm_scene.h>

#include <rply.h>   //.ply parser

#define DEBUG_BOF
// ============================== PLY ==============================

class bof_fileio_parsed_ply_
{
public:
  vgl_box_3d<double> bbox;
  double p[3];
  vcl_vector<int > vertex_indices;
};

// ============================== End PLY ==============================


//: Call-back function for a "vertex" element
int bof_plyio_vertex_cb_(p_ply_argument argument);


bof_scene_categories::bof_scene_categories(vcl_string bof_dir):info_(bof_info(bof_dir)),path_out_(bof_dir), bbox_scale_(10.0)
{
  vcl_ifstream xml_ifs(xml_path().c_str());
  if(!xml_ifs.is_open()){
    vcl_cerr << "Error: Could not open xml info file: " << xml_path() << " \n";
    throw;
  }
  vcl_cout << "Parsing: " << xml_path() << "\n";
  
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("bof_category_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cerr << "Error: bof_info - could not parse xml root\n";
    throw;
  }
  
  //Get number of categories
  bxml_element prop_query("properties");
  bxml_data_sptr prop_data = bxml_find_by_name(root, prop_query);
  
  bxml_element* prop_elm = dynamic_cast<bxml_element*>(prop_data.ptr());
  unsigned ncategories;
  prop_elm->get_attribute("ncategories", ncategories);
  category_names_.clear();
  category_names_.resize(ncategories, "none");
  
  category_scenes_.clear();
  category_scenes_.resize(ncategories);
  
                          
  vcl_cout << "Number of categories: " << ncategories << vcl_endl;
  
  //parse scenes
  vcl_vector<bxml_data_sptr> scene_data = bxml_find_all_with_name(root, bxml_element("scene"));
  unsigned n_scenes = scene_data.size();
  ply_paths_.clear();
  ply_paths_.resize(n_scenes);
  vcl_cout << "Number of scenes: " << n_scenes << vcl_endl;
  for (unsigned si=0; si<n_scenes; si++) {
    bxml_element* scene_elm = dynamic_cast<bxml_element*>(scene_data[si].ptr());
    int scene_id = -1;
    vcl_string path;
    unsigned n = 0;
    scene_elm->get_attribute("id", scene_id);
    scene_elm->get_attribute("path", path);
    
    if (info_.scenes_[scene_id]!=path) {
      vcl_cerr << "Scene path should be: " << info_.scenes_[scene_id]<< ", but it is: " <<path << vcl_endl;
      throw;
    }

    vcl_vector<bxml_data_sptr> category_data = bxml_find_all_with_name(scene_elm, bxml_element("object"));
    
    vcl_vector<bof_scene_object> scene_ply_paths;
    

    for (unsigned ci = 0; ci < category_data.size(); ci++) {
      bxml_element* category_elm = dynamic_cast<bxml_element*>(category_data[ci].ptr());
      int class_id = -1;
      vcl_string name;
      vcl_string ply_path;
      category_elm->get_attribute("class_id", class_id);
      category_elm->get_attribute("class_name", name);
      category_elm->get_attribute("ply_path", ply_path);
      
      if(class_id<0)
        continue;
      
      category_scenes_[class_id].insert(si);
      

      
      //set the category name
      category_names_[class_id]= name; 
     // vcl_cout << "Object " << ci << " is " << scene_ply_paths.size() << vcl_endl;

      bof_scene_object bof_obj; bof_obj.ply_path = ply_path; bof_obj.class_id = class_id;
      scene_ply_paths.push_back(bof_obj);                             
    }
    vcl_cout << "Number of objects in scene " << scene_id << " is " << scene_ply_paths.size() << vcl_endl;

    ply_paths_[scene_id] = scene_ply_paths;
  }
  
  for (unsigned i=0; i<ncategories; i++) {
    vcl_cout << "Category " << i << " is "<< category_names_[i] << "\n";
  }
  
}

bool bof_scene_categories::load_bbox_from_ply(const vcl_string &ply_file, vgl_box_3d<double> &box)
{
  long nvertices;
  
  bof_fileio_parsed_ply_ parsed_ply;
  parsed_ply.bbox = box;
  
  // OPEN file
  vcl_cerr << " loading " << ply_file << " :\n";
  p_ply ply = ply_open(ply_file.c_str(), NULL, 0, NULL);
  if (!ply)
    return false;
  
  // Read HEADER
  if (!ply_read_header(ply)) return false;
  
  // vertex
  nvertices =
  ply_set_read_cb(ply, "vertex", "x",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 2);
  
  
  vcl_cerr << nvertices << " points \n";
  
  // Read DATA
  if (!ply_read(ply)) return false;
  
  // CLOSE file
  ply_close(ply);
  
  box=parsed_ply.bbox;

  vcl_cerr << "  done.\n";
  
}


bool bof_scene_categories::scale_ply(const vcl_string &ply_file_in, const vcl_string &ply_file_out, const double scale )
{
  
  long nvertices;
  
  bof_fileio_parsed_ply_ parsed_ply;
 
  // OPEN file
  p_ply ply = ply_open(ply_file_in.c_str(), NULL, 0, NULL);
  if (!ply)
    return false;
  
  // Read HEADER
  if (!ply_read_header(ply)) return false;
  
  // vertex
  nvertices =
  ply_set_read_cb(ply, "vertex", "x",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z",
                  bof_plyio_vertex_cb_, (void*) (&parsed_ply), 2);
  
  
  vcl_cerr << " loading " << ply_file_in << " :\n\t" << nvertices << " points \n";
  
  // Read DATA
  if (!ply_read(ply)) return false;
  
  // CLOSE file
  ply_close(ply);
  
  vgl_box_3d<double> box=parsed_ply.bbox;
  
//  if(box.height() <1.0e-7 || box.width() < 1.0e-7){
//    vcl_cerr << "Skipping: " << ply_file_out << " input box has 0-dim\n";
//    return false;
//  }
  
  // OPEN output file
  p_ply oply = ply_create(ply_file_out.c_str(), PLY_ASCII, NULL, 0, NULL);
  
  vcl_cerr << "  saving " << ply_file_out << " :\n";

  
  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", 2);
  ply_add_scalar_property(oply, "x", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "y", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "z", PLY_DOUBLE); //PLY_FLOAT
  // comment
  ply_add_comment(oply, "created by dbrec3d_scale_ply");
  // object info
  ply_add_obj_info(oply, "a vgl_box_3d<double> object");
  // end header
  ply_write_header(oply);
  
  // DATA SECTION
  // save min and max boint of the box to ply file
  ply_write(oply, box.min_x()*scale);
  ply_write(oply, box.min_y()*scale);
  ply_write(oply, box.min_z()*scale-5.0);
  ply_write(oply, box.max_x()*scale);
  ply_write(oply, box.max_y()*scale);
  ply_write(oply, box.max_z()*scale+5.0);

  
  // CLOSE PLY FILE
  ply_close(oply);
  vcl_cerr << "  done.\n";
  return true; 
  
  
}

//: Function for debugging. Note that the labeled scene puts 255 whenever a voxel belongs to an object
//  Category labels may not be known so only use this function for debugging purposes
void bof_examine_ground_truth(boxm_scene<boct_tree<short, char> > *labeled_scene,
                              double finest_cell_length,
                              const vcl_vector<vcl_string> &ply_paths,
                              const vcl_string &class_name)
{
  
  typedef boct_tree<short, char> char_tree_type;
  typedef boct_tree_cell<short, char> char_cell_type;
  
  if(!(labeled_scene))
  {
    vcl_cerr << "Error in bof_examine_ground_truth --> Could not cast scene" << vcl_endl;
    return;
  }
  
  vcl_cout << "Number of objects for this scene is " << ply_paths.size() <<vcl_endl;;
  
  vcl_vector<vcl_string>::const_iterator it = ply_paths.begin();
  for (; it!=ply_paths.end(); it++) {
    
    
    vgl_box_3d<double> outer_bbox;
    vgl_box_3d<double> inner_bbox;
    vgl_box_3d<double> tight_bbox;
    
    bof_scene_categories::load_bbox_from_ply(*it, tight_bbox);
    outer_bbox = tight_bbox;
    outer_bbox.expand_about_centroid(finest_cell_length*10.0);

    
    vcl_cout << "Object's tight box: " << tight_bbox << "\n"
    << "Object's outer box: " << outer_bbox << "\n";
    
    vcl_vector<char_cell_type*> labeled_leaves;
    labeled_scene->leaves_in_region(outer_bbox,labeled_leaves);
    
    vcl_cout << "Number of samples in the object: " << labeled_leaves.size() << "\n";
    
 
    for (unsigned i=0; i<labeled_leaves.size(); i++) {
      if(labeled_leaves[i]->level() == 0)
        labeled_leaves[i]->set_data(127);
    }
    
    //write the scene without unloading the blocks
    labeled_scene->write_active_blocks(false);
    
  }

  labeled_scene->unload_active_blocks();
}

//: Learn the joint P(X,C) voxels of this scene inside the object with appropiate category
bool bof_scene_categories::learn_categories(bof_codebook_sptr codebook, unsigned scene_id, bof_labels_keypoint_joint &p_cx, vcl_string path_out)
{
  if(!info_.training_scenes_[scene_id])
    return false;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  typedef boct_tree<short, char> char_tree_type;
  typedef boct_tree_cell<short, char> char_cell_type;
  
  //Load the labels scene for this id
  boxm_scene_base_sptr scene_base = info_.load_feature_scene(scene_id);
  boxm_scene<feature_tree_type>* scene = dynamic_cast<boxm_scene<feature_tree_type>*> (scene_base.as_pointer());
  
  boxm_scene_base_sptr class_id_scene_base = info_.load_category_scene(scene_id);
  boxm_scene<char_tree_type>* class_id_scene = dynamic_cast<boxm_scene<char_tree_type>*> (class_id_scene_base.as_pointer());
  
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  
  if(!(scene && class_id_scene && valid_scene))
  {
    vcl_cerr << "Error in bof_scene_categories::label_objects --> Could not cast scene" << vcl_endl;
    return false;
  }
    
  vcl_cout << "Number of objects for scene " << scene_id << " is " << ply_paths_[scene_id].size() <<vcl_endl;
  
  if(ply_paths_[scene_id].empty())
    return false;
  
  vcl_vector<bof_scene_object>::iterator it = ply_paths_[scene_id].begin();
  for (; it!=ply_paths_[scene_id].end(); it++) {
    
    if (it->class_id < 0) {
      continue;
    }
    
    char class_id = it->class_id;
    
    vgl_box_3d<double> outer_bbox;
    vgl_box_3d<double> tight_bbox;
    
    load_bbox_from_ply(it->ply_path, tight_bbox);
    outer_bbox = tight_bbox;
    outer_bbox.expand_about_centroid(info_.finest_cell_length_[scene_id]*this->bbox_scale_);
    
#ifdef DEBUG_BOF
    vcl_cout << "Object's tight box: " << tight_bbox << "\n"
    << "Object's outer box: " << outer_bbox << "\n";

#endif
   
    vcl_vector<feature_cell_type*> object_leaves;
    vcl_vector<char_cell_type*> label_leaves;
    vcl_vector<boct_tree_cell<short, bool>* > valid_leaves;
    scene->leaves_in_region(outer_bbox, object_leaves);
    class_id_scene->leaves_in_region(outer_bbox,label_leaves);
    valid_scene->leaves_in_region(outer_bbox, valid_leaves);
    
#ifdef DEBUG_BOF
    vcl_cout << "Number of leaves in the object: " << object_leaves.size() << "\n";
#endif
    
    vcl_vector<vnl_vector_fixed<double, 10> > leaves_data;
    
    for (unsigned i=0; i<object_leaves.size(); i++) {
      if((object_leaves[i]->level() == 0) && valid_leaves[i]->data()){
        const vnl_vector_fixed<double, 10> &data = object_leaves[i]->data();
        label_leaves[i]->set_data(class_id);
        leaves_data.push_back(data); 
      }    
    }
    

    vcl_cout << "Number of valid leaves (at level 0) in the object: " << leaves_data.size() << "\n";

    
    if(leaves_data.size()==0)
      continue;
    
    //write the scene without unloading the blocks
    class_id_scene->write_active_blocks(false);
    
    
    vcl_vector<dbcll_euclidean_cluster_light<10> > clusters;
    dbcll_compute_euclidean_clusters(leaves_data,codebook->means_, clusters);
    
    vcl_stringstream mesh_ss;
    mesh_ss << path_out << "/" << vul_file::strip_extension(vul_file::strip_directory(it->ply_path)) << ".xml";
    dbcll_xml_write(clusters, mesh_ss.str());
    
    
    p_cx.merge_clusters(class_id, clusters);

  }
  scene->unload_active_blocks();
  class_id_scene->unload_active_blocks();
  
  return true;
}

//: Given an object in a scene and the joint P(X,C), choose the maximum aposteriory category.
void bof_scene_categories::classify(bof_codebook_sptr codebook, unsigned scene_id, 
                                    const bof_labels_keypoint_joint &p_cx, vcl_string classification_dir)
{
  if(info_.training_scenes_[scene_id])
    return;
  
  if(ply_paths_[scene_id].size() ==0)
    return;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  typedef boct_tree<short, char> char_tree_type;
  typedef boct_tree_cell<short, char> char_cell_type;
  
  //Load the labels scene
  boxm_scene_base_sptr scene_base = info_.load_feature_scene(scene_id);
  boxm_scene<feature_tree_type>* scene = dynamic_cast<boxm_scene<feature_tree_type>*> (scene_base.as_pointer());
   
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
    
  if(!(scene))
  {
    vcl_cerr << "Error in bof_scene_categories::label_objects --> Could not cast scene" << vcl_endl;
    return;
  }
   
  vcl_cout << "Number of objects for scene " << scene_id << " is " << ply_paths_[scene_id].size() <<vcl_endl;;
                                      
  vcl_vector< vcl_vector<double> > log_p_c_given_obj;
  
  vcl_vector<bof_scene_object>::iterator it = ply_paths_[scene_id].begin();
  
  //file to write classification results
  vcl_stringstream cl_ss;
  cl_ss << classification_dir << "/scene_" << scene_id; 
  
  if(!vul_file::exists(cl_ss.str()))
    vul_file::make_directory(cl_ss.str());
  
  cl_ss<< "/classification.txt";
  vcl_ofstream cl_ofs(cl_ss.str().c_str());
  
  for (; it!=ply_paths_[scene_id].end(); it++) {
    if (it->class_id < 0) {
      continue;
    }
    
    char class_id = it->class_id;
    
    vgl_box_3d<double> outer_bbox;
    vgl_box_3d<double> tight_bbox;
    
    load_bbox_from_ply(it->ply_path, tight_bbox);
    outer_bbox = tight_bbox;
    outer_bbox.expand_about_centroid(info_.finest_cell_length_[scene_id]*this->bbox_scale_);    
#ifdef DEBUG_BOF
    vcl_cout << "Object's tight box: " << tight_bbox << "\n"
    << "Object's outer box: " << outer_bbox << "\n";
#endif
    
    vcl_vector<feature_cell_type*> object_leaves;
    scene->leaves_in_region(outer_bbox, object_leaves);
    vcl_vector<boct_tree_cell<short, bool>* > valid_leaves;
    valid_scene->leaves_in_region(outer_bbox, valid_leaves);
    
#ifdef DEBUG_BOF
    vcl_cout << "Number of leaves in the object: " << object_leaves.size() << "\n";
#endif
    
    vcl_vector<vnl_vector_fixed<double, 10> > leaves_data;
    
    for (unsigned i=0; i<object_leaves.size(); i++) {
       if((object_leaves[i]->level() == 0) && valid_leaves[i]->data()){
          const vnl_vector_fixed<double, 10> &data = object_leaves[i]->data();
          leaves_data.push_back(data);   
       }
    }
    

    vcl_cout << "Number of  valid leaves at level 0 in the object: " << leaves_data.size() << "\n";
    
    if(leaves_data.size()==0)
      continue;
    
    vcl_vector<dbcll_euclidean_cluster_light<10> > clusters;
    dbcll_compute_euclidean_clusters(leaves_data,codebook->means_, clusters);
    
    
    vcl_vector<double> log_p_c_given_obj_ith;
    char max_c = p_cx.max_aposteriori(clusters, log_p_c_given_obj_ith);
    
    cl_ofs << (int)class_id << " " << (int)max_c << "\n";
    
    //write to file
    vcl_stringstream ss;
    ss << classification_dir << "/scene_" << scene_id << "/";
    
    vcl_stringstream mesh_ss;
    mesh_ss << classification_dir << "/scene_" << scene_id << "/" << vul_file::strip_extension(vul_file::strip_directory(it->ply_path)) << ".xml";
    dbcll_xml_write(clusters, mesh_ss.str());
    
    ss << "aposteriori_" << vul_file::strip_extension(vul_file::strip_directory(it->ply_path)) << ".txt";
    
    vcl_ofstream ofs(ss.str().c_str());
    if(!ofs.is_open()){
      vcl_cerr << "Failed to open " << ss.str() << " for write\n";
      continue;
    }
    
    ofs << int(max_c) << " ";
    ofs.precision(15);
    for (unsigned pi=0; pi < log_p_c_given_obj_ith.size(); pi++) {
      ofs << log_p_c_given_obj_ith[pi] << " ";
    }
    ofs.close();
  }
  cl_ofs.close();
  scene->unload_active_blocks();
}


//: Compute representative bounding box for each category
void bof_scene_categories::bounding_box_statictics()
{
  
  vcl_vector<double> avg_bbox_xy(nclasses(), 0.0);
  vcl_vector<double> avg_bbox_yz(nclasses(), 0.0);
  vcl_vector<double> avg_bbox_x(nclasses(), 0.0);
  
  vcl_vector<double> var_bbox_xy(nclasses(), 0.0);
  vcl_vector<double> var_bbox_yz(nclasses(), 0.0);
  vcl_vector<double> var_bbox_x(nclasses(), 0.0);

  vcl_vector<unsigned> n_objects(nclasses(), 0);
  
  for (unsigned scene_id = 0; scene_id < nscenes(); scene_id++) {
    
    if(ply_paths_[scene_id].empty())
      continue;
    
    
    vcl_vector<bof_scene_object>::iterator it = ply_paths_[scene_id].begin();
    for (; it!=ply_paths_[scene_id].end(); it++) {
      
      if (it->class_id < 0) {
        continue;
      }
      
      char class_id = it->class_id;
      
      vgl_box_3d<double> outer_bbox;
      vgl_box_3d<double> tight_bbox;
      
      load_bbox_from_ply(it->ply_path, tight_bbox);
      outer_bbox = tight_bbox;
      outer_bbox.expand_about_centroid(info_.finest_cell_length_[scene_id]*this->bbox_scale_);
      
      
      double x = outer_bbox.width();
      double y = outer_bbox.height();
      double z = outer_bbox.depth();
      double xy = x/y;
      double yz = y/z;
      
      avg_bbox_xy[class_id] +=  xy;
      avg_bbox_yz[class_id] +=  yz;
      avg_bbox_x[class_id] += x;
      
      var_bbox_xy[class_id] +=  xy * xy;
      var_bbox_yz[class_id] +=  yz * yz;
      var_bbox_x[class_id] += x * x;
      
      n_objects[class_id] ++;
    }
  }
  
  for (unsigned class_id = 0 ; class_id < nclasses(); class_id++) {
    avg_bbox_xy[class_id] = avg_bbox_xy[class_id]/(double)n_objects[class_id];
    avg_bbox_yz[class_id] = avg_bbox_yz[class_id]/(double)n_objects[class_id];
    avg_bbox_x[class_id] = avg_bbox_x[class_id]/(double)n_objects[class_id];
    
    var_bbox_xy[class_id] =  var_bbox_xy[class_id]/(double)n_objects[class_id]  -  avg_bbox_xy[class_id];
    var_bbox_yz[class_id] =  var_bbox_yz[class_id]/(double)n_objects[class_id]  -  avg_bbox_yz[class_id];
    var_bbox_x[class_id] =  var_bbox_x[class_id]/(double)n_objects[class_id]  -  avg_bbox_x[class_id];
    
    
    vcl_cout << "***************************************\n"
    << "Class: " << category_names_[class_id] << "\n"
    << "Average x:y ratio: " << avg_bbox_xy[class_id] << "\n"
    << "Average y:z ratio: " << avg_bbox_yz[class_id] << "\n"
    << "Average x length: " << avg_bbox_x[class_id] << "\n";

    double typical_width = avg_bbox_x[class_id] + vcl_sqrt(var_bbox_x[class_id]);
    double typical_height = typical_width/avg_bbox_xy[class_id];
    double typical_depth = typical_height/var_bbox_yz[class_id];
    
    vgl_box_3d<double> typical_bbox(vgl_point_3d<double>(0.0,0.0,0.0),typical_width, typical_height, typical_depth, vgl_box_3d<double>::centre); 
    
    
    //Write a characteristic bounding box
    vcl_stringstream ply_file_out;
    ply_file_out << path_out_ << "/typical_bbox";
    
    if(!vul_file::exists(ply_file_out.str()))
      vul_file::make_directory(ply_file_out.str());
    
    ply_file_out << "/bbox_class_" << class_id << ".ply";
    
    // OPEN output file
    p_ply oply = ply_create(ply_file_out.str().c_str(), PLY_ASCII, NULL, 0, NULL);
    
    vcl_cerr << "  saving " << ply_file_out << " :\n";
    
    
    // HEADER SECTION
    // vertex
    ply_add_element(oply, "vertex", 2);
    ply_add_scalar_property(oply, "x", PLY_DOUBLE); //PLY_FLOAT
    ply_add_scalar_property(oply, "y", PLY_DOUBLE); //PLY_FLOAT
    ply_add_scalar_property(oply, "z", PLY_DOUBLE); //PLY_FLOAT
                                                    // comment
    ply_add_comment(oply, "created by dbrec3d_scale_ply");
    // object info
    ply_add_obj_info(oply, "a vgl_box_3d<double> object");
    // end header
    ply_write_header(oply);
    
    // DATA SECTION
    // save min and max boint of the box to ply file
    ply_write(oply, typical_bbox.min_x());
    ply_write(oply, typical_bbox.min_y());
    ply_write(oply, typical_bbox.min_z());
    ply_write(oply, typical_bbox.max_x());
    ply_write(oply, typical_bbox.max_y());
    ply_write(oply, typical_bbox.max_z());
    
    
    // CLOSE PLY FILE
    ply_close(oply);
    vcl_cerr << "  done.\n";

  }
}


//: Load all objects for the given scene
void bof_scene_categories::load_objects(unsigned scene_id, vcl_vector<vgl_box_3d<double> > &bboxes)
{
  bboxes.clear();
  vcl_vector<bof_scene_object>::iterator it = ply_paths_[scene_id].begin();
  for (; it!=ply_paths_[scene_id].end(); it++) 
  {    
    vgl_box_3d<double> outer_bbox;
    vgl_box_3d<double> tight_bbox;
    
    load_bbox_from_ply(it->ply_path, tight_bbox);
    outer_bbox = tight_bbox;
    outer_bbox.expand_about_centroid(info_.finest_cell_length_[scene_id]*bbox_scale_);
    bboxes.push_back(outer_bbox);
    vcl_cout << outer_bbox << vcl_endl;
  }
  
}


// Function to init the xml infp file
void bof_init_scene_categories_xml(vcl_string bof_dir)
{
  bof_info info(bof_dir);
  info.init_category_scenes(-1);
  
  //If file bof_category_info.xml doesn't exist, initialize. Otherwise don't overwrite
  if(!vul_file::exists((bof_dir + "/bof_category_info.xml")))
  {
    bxml_document doc;
    bxml_element *root = new bxml_element("bof_category_info");
    doc.set_root_element(root);
    root->append_text("\n");
    
    bxml_element* prop_elm = new bxml_element("properties");
    prop_elm->append_text("\n");
    prop_elm->set_attribute("ncategories", 0);
    
    root->append_data(prop_elm);
    root->append_text("\n");
    
    //write the scenes
    for (unsigned si = 0 ; si < info.nscenes(); si++) {
      bxml_element* scene_elm = new bxml_element("scene");
      scene_elm->append_text("\n");
      scene_elm->set_attribute("id", si);
      scene_elm->set_attribute("path", info.scenes_[si]);
      
      bxml_element* dummy_c = new bxml_element("object");
      dummy_c->append_text("\n");
      dummy_c->set_attribute("class_id", -1);
      dummy_c->set_attribute("class_name", "");
      dummy_c->set_attribute("ply_path", "");
      
      scene_elm->append_data(dummy_c);
      scene_elm->append_text("\n");

      
      root->append_data(scene_elm);
      root->append_text("\n");
    }
    
    //write to disk  
    vcl_ofstream os((bof_dir + "/bof_category_info.xml").c_str());
    bxml_write(os, doc);
    os.close();  
  }
  
}

// ============================== PLY ==============================


//: Call-back function for a "vertex" element
int bof_plyio_vertex_cb_(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);
  
  bof_fileio_parsed_ply_* parsed_ply =  (bof_fileio_parsed_ply_*) temp;
  
  switch (index)
  {
    case 0: // "x" coordinate
      parsed_ply->p[0] = ply_get_argument_value(argument);
      break;
    case 1: // "y" coordinate
      parsed_ply->p[1] = ply_get_argument_value(argument);
      break;
    case 2: // "z" coordinate
      parsed_ply->p[2] = ply_get_argument_value(argument);
    { // INSERT VERTEX INTO THE MESH
      parsed_ply->bbox.add(vgl_point_3d<double>(parsed_ply->p));
      break;
    }
    default:
      assert(!"This should not happen: index out of range");
  };
  return 1;
}
                          

// ============================== End PLY ==============================

/****************** BInary IO fo bof_scene_categories**************************/
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & , bof_scene_categories const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & , bof_scene_categories & )
{
  vcl_cerr << "Error: Trying to read but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & , const bof_scene_categories & )
{
  vcl_cerr << "Error: Trying to print but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bof_scene_categories* p)
{
  delete p;
  vcl_cerr << "Error: Trying to read but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const bof_scene_categories* &p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(vcl_ostream& os, const bof_scene_categories* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}