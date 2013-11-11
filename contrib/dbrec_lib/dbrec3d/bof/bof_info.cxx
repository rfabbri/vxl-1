//:
// \file
// \author Isabel Restrepo
// \date 1-Apr-2011

#include "bof_info.h"

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <boxm/boxm_scene_parser.h>

#include <vul/vul_file.h>

#include  <bvpl/bvpl_octree/bvpl_global_pca.h>
#include  <bvpl/bvpl_octree/bvpl_global_taylor.h>

//: Constructor - from global_pca or global_taylor xml info file and number of means. It also writes this class' info file
bof_info::bof_info(vcl_string global_dir, vcl_string bof_dir)
{
  if(vul_file::exists(global_dir + "/pca_global_info.xml")){
    vcl_cout << "Initializing BOF info from global PCA \n";
    bvpl_global_pca<125> global_pca(global_dir);
    path_out_ = bof_dir;
    aux_dirs_= global_pca.aux_dirs();
    finest_cell_length_ = global_pca.cell_lengths();
    training_scenes_ = global_pca.training_scenes();
    scenes_.clear();
    for (unsigned i =0; i<aux_dirs_.size(); i++) {
      vcl_stringstream proj_scene_ss;
      proj_scene_ss << aux_dirs_[i] << "/proj_pca_scene_" << i << ".xml";
      scenes_.push_back(proj_scene_ss.str());
    }
    xml_write();
  }
  else if(vul_file::exists(global_dir + "/taylor_global_info.xml")){
    vcl_cout << "Initializing BOF info from global Taylor \n";
    const vcl_string kernel_names[10] = {"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz" };
    bvpl_global_taylor<double, 10> global_taylor(global_dir, kernel_names);
    path_out_ = bof_dir;
    aux_dirs_= global_taylor.aux_dirs();
    finest_cell_length_ = global_taylor.cell_lengths();
    //training_scenes_ = global_pca.training_scenes();
    scenes_.clear();
    for (unsigned i =0; i<aux_dirs_.size(); i++) {
      vcl_stringstream proj_scene_ss;
      proj_scene_ss << aux_dirs_[i] << "/proj_taylor_scene_" << i << ".xml";
      scenes_.push_back(proj_scene_ss.str());
    }
    xml_write();
  }
  else
      vcl_cerr << "Could not find global info file \n";
}

//: Constructor - from xml info file
bof_info::bof_info(vcl_string bof_dir)
{
  path_out_ = bof_dir;
  vcl_ifstream xml_ifs(xml_path().c_str());
  if(!xml_ifs.is_open()){
    vcl_cerr << "Error: bvpl_discover_pca_kernels - could not open xml info file: " << xml_path() << " \n";
    throw;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("bof_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cerr << "Error: bof_info - could not parse xml root\n";
    throw;
  }

  //Parse scenes
  bxml_element scenes_query("scene");
  vcl_vector<bxml_data_sptr> scenes_data = bxml_find_all_with_name(root, scenes_query);
  
  unsigned nscenes = scenes_data.size();
  vcl_cout<<"Number of scenes " << nscenes<< "\n";

  scenes_.clear();
  scenes_.resize(nscenes);
  
  aux_dirs_.clear();
  aux_dirs_.resize(nscenes);
  
  finest_cell_length_.clear();
  finest_cell_length_.resize(nscenes);

  for(unsigned si = 0; si < nscenes; si++)
  {
    bxml_element* scenes_elm = dynamic_cast<bxml_element*>(scenes_data[si].ptr());
    int id = -1;
    scenes_elm->get_attribute("id", id);
    if(id<0)
      continue;
    scenes_elm->get_attribute("path", scenes_[id]);
    scenes_elm->get_attribute("aux_dir", aux_dirs_[id]);
    scenes_elm->get_attribute("cell_length" , finest_cell_length_[id]);
    vcl_cout << "Scene " << id << " is " << scenes_[id] << "\n";
    
  }  
  
  //bxml_element category_query("category");
//  vcl_vector<bxml_data_sptr> category_data = bxml_find_all_with_name(root, category_query);
//  unsigned num_cat = category_data.size();
//  
//  categories_.clear();
//  categories_.resize(num_cat);
//  category_names_.clear();
//  category_names_.resize(num_cat);
//  
//  for (unsigned ci=0; ci<num_cat; ci++) {
//    bxml_element* category_elm = dynamic_cast<bxml_element*>(category_data[ci].ptr());
//    int id = -1;
//    vcl_string name;
//    unsigned n = 0;
//    category_elm->get_attribute("id", id);
//    if(id<0)
//      continue;
//    category_elm->get_attribute("name", name);
//    category_elm->get_attribute("nscenes", n);
//    
//    //read out the scenes
//    vcl_vector<unsigned> s_id;
//    for (bxml_element::const_data_iterator s_it = category_elm->data_begin(); s_it != category_elm->data_end(); s_it++) {
//      if ((*s_it)->type() == bxml_data::TEXT) {
//        bxml_text* t = dynamic_cast<bxml_text*>((*s_it).ptr());
//        vcl_stringstream text_d(t->data()); vcl_string buf;
//        vcl_vector<vcl_string> tokens;
//        while (text_d >> buf) {
//          tokens.push_back(buf);
//        }
//        if (tokens.size() != n)
//          continue;
//        for (unsigned i = 0; i < n; i++) {
//          vcl_stringstream ss2(tokens[i]); int s_type_id;
//          ss2 >> s_type_id;
//          s_id.push_back(s_type_id);
//        }
//        break;
//      }
//    }
//    categories_[id] = s_id;
//    category_names_[id] = name;
//    
//    vcl_cout << "Category: " << id << ", whith name: " << name << " has: " <<n <<" scenes aasocited with it \n";
//  }  
  
  bxml_element* train_elm = dynamic_cast<bxml_element*>(bxml_find_by_name(root, bxml_element("training_scenes")).ptr());
  unsigned n_train_scenes = 0;
  train_elm->get_attribute("nscenes", n_train_scenes);
  training_scenes_.clear();
  training_scenes_.resize(nscenes, false);
                         
  //read out the scenes
  for (bxml_element::const_data_iterator s_it = train_elm->data_begin(); s_it != train_elm->data_end(); s_it++) {
    if ((*s_it)->type() == bxml_data::TEXT) {
      bxml_text* t = dynamic_cast<bxml_text*>((*s_it).ptr());
      vcl_stringstream text_d(t->data()); vcl_string buf;
      vcl_vector<vcl_string> tokens;
      while (text_d >> buf) {
        tokens.push_back(buf);
      }
      if (tokens.size() != n_train_scenes)
        continue;
      for (unsigned i = 0; i < n_train_scenes; i++) {
        vcl_stringstream ss2(tokens[i]); int s_type_id;
        ss2 >> s_type_id;
        vcl_cout << "Scene: " << s_type_id << " is used for training \n";
        training_scenes_[s_type_id]=true;
      }
      break;
    }
  }
 
}

void bof_info::init_category_scenes(char label)
{
  
  vcl_cout << "Initializing " << aux_dirs_.size() << " category scenes \n";
  for(unsigned i = 0; i < aux_dirs_.size(); i++)
  {
    boxm_scene_base_sptr valid_scene_base = load_valid_scene(i);
    boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
    if (!valid_scene){
      vcl_cerr << "Error in bof_info::init_category_scene: Could not cast valid scene \n";
      return;
    }
    
    vcl_stringstream scene_ss;
    scene_ss << "category_scene_" << i ;
    vcl_string scene_path = aux_dirs_[i] + "/" + scene_ss.str() + ".xml";
    
    vcl_cout << "Initializing " << scene_path << "\n";

   
    if(!vul_file::exists(scene_path)){
      vcl_cout<< "Scene: " << scene_path << " does not exist, initializing -xml and data" << vcl_endl;
      typedef boct_tree<short,char > tree_type;
      boxm_scene<boct_tree<short, char> > *scene =
      new boxm_scene<boct_tree<short, char> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
      scene->set_appearance_model(BOXM_CHAR);
      scene->set_paths(aux_dirs_[i], scene_ss.str());
      scene->write_scene("/" + scene_ss.str() +  ".xml");
      //initialize class label to -1 
      valid_scene->clone_blocks_to_type<boct_tree<short,char> >(*scene, label);
     
    }
  }
}

//: Load scene of features
boxm_scene_base_sptr bof_info::load_feature_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr scene_base = new boxm_scene_base();
  boxm_scene_parser scene_parser;
  scene_base->load_scene(scenes_[scene_id], scene_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, vnl_vector_fixed<double,10> > > *scene= new boxm_scene<boct_tree<short, vnl_vector_fixed<double,10> > >();
  if (scene_base->appearence_model() == VNL_DOUBLE_10){     
    scene->load_scene(scene_parser);
    scene_base = scene;
  }else {
    vcl_cerr << "Error in bof_info::load_feature_scene: Invalid apperance model:" << scene_base->appearence_model() << vcl_endl;
    return NULL;
  }
  
  return scene_base;
  
}

//: Load auxiliary scene indicating if a cell is valid. e.g border cells are not valid
boxm_scene_base_sptr bof_info::load_valid_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/valid_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL){     
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }else {
    vcl_cerr << "Error in bvpl_global_pca::load_aux_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return aux_scene_base;  
}

#if 0 //scenes themselves are split into train/test and as oppossed to intermixing voxels in a scene
//: Load auxiliary scene indicating if a cell is should be used for training
boxm_scene_base_sptr bof_info::load_train_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/train_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL){     
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }else {
    vcl_cerr << "Error in bvpl_global_pca::load_aux_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return aux_scene_base;  
}
#endif

//: Load auxiliary scene containing an id for the class category
boxm_scene_base_sptr bof_info::load_category_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bof_info::load_category_scene: Invalid scene id" << vcl_endl;
    return NULL;
    }  
  
    //base class scene
    boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  
    vcl_stringstream aux_scene_ss;
    aux_scene_ss << aux_dirs_[scene_id] << "/category_scene_" << scene_id << ".xml";
  
    
    if(!vul_file::exists(aux_scene_ss.str())){
      vcl_cout<< "Scene: " << aux_scene_ss.str() << " does not exist, initializing -xml and data" << vcl_endl;
      
      boxm_scene_base_sptr valid_scene_base = load_valid_scene(scene_id);
      boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
      if (!valid_scene){
        vcl_cerr << "Error in bof_info::load_category_scene: Could not cast valid scene \n";
        return NULL;
      }
      vcl_stringstream scene_ss;
      scene_ss << "category_scene_" << scene_id ;
      boxm_scene<boct_tree<short, char> > *aux_scene =
      new boxm_scene<boct_tree<short, char> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
      aux_scene->set_appearance_model(BOXM_CHAR);
      aux_scene->set_paths(aux_dirs_[scene_id], scene_ss.str());
      aux_scene->write_scene(scene_ss.str() +  ".xml");
      //initialize class label to -1 
      valid_scene->clone_blocks_to_type<boct_tree<short,char> >(*aux_scene, -1);
      aux_scene_base = aux_scene;
    }
    else
    {
      //load scene
      boxm_scene_parser aux_parser;
      aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
      
      //cast scene
      boxm_scene<boct_tree<short, char > > *aux_scene= new boxm_scene<boct_tree<short, char > >();
      if (aux_scene_base->appearence_model() == BOXM_CHAR){     
        aux_scene->load_scene(aux_parser);
        aux_scene_base = aux_scene;
      }else {
        vcl_cerr << "Error in bof_info::load_category_scene: Invalid apperance model" << vcl_endl;
        return NULL;
      }
    }
    
    return aux_scene_base;  
}

//: Load auxiliary scene containing an id for the vocabulary cluster or visual word
boxm_scene_base_sptr bof_info::load_cluster_id_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bof_info::load_cluster_id_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  
  //base class scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/cluster_id_scene_" << scene_id << ".xml";
  
  
  if(!vul_file::exists(aux_scene_ss.str())){
    vcl_cout<< "Scene: " << aux_scene_ss.str() << " does not exist, initializing -xml and data" << vcl_endl;
    
    boxm_scene_base_sptr valid_scene_base = load_valid_scene(scene_id);
    boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
    if (!valid_scene){
      vcl_cerr << "Error in bof_info::load_cluster_id_scene: Could not cast valid scene \n";
      return NULL;
    }
    vcl_stringstream scene_ss;
    scene_ss << "cluster_id_scene_" << scene_id ;
    boxm_scene<boct_tree<short, short> > *aux_scene =
    new boxm_scene<boct_tree<short, short> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), 1);
    aux_scene->set_appearance_model(BOXM_SHORT);
    aux_scene->set_paths(aux_dirs_[scene_id], scene_ss.str());
    aux_scene->write_scene(scene_ss.str() +  ".xml");
    //initialize class label to -1 
    aux_scene_base = aux_scene;
  }
  else
  {
    //load scene
    boxm_scene_parser aux_parser;
    aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
    
    //cast scene
    boxm_scene<boct_tree<short, short > > *aux_scene= new boxm_scene<boct_tree<short, short > >();
    if (aux_scene_base->appearence_model() == BOXM_SHORT){     
      aux_scene->load_scene(aux_parser);
      aux_scene_base = aux_scene;
    }else {
      vcl_cerr << "Error in bof_info::load_cluster_id_scene: Invalid apperance model" << vcl_endl;
      return NULL;
    }
  }
  
  return aux_scene_base;  
}

//: Write this class to xml file
void bof_info::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("bof_info");
  doc.set_root_element(root);
  root->append_text("\n");
  
  //write the scenes
  for(unsigned i =0; i<scenes_.size(); i++) 
  {
    vcl_cout << "Here too" <<vcl_endl;
    bxml_element* scenes_elm = new bxml_element("scene");
    scenes_elm->append_text("\n");
    scenes_elm->set_attribute("id", i);
    scenes_elm->set_attribute("path", scenes_[i]);
    scenes_elm->set_attribute("aux_dir", aux_dirs_[i]);
    scenes_elm->set_attribute("cell_length", finest_cell_length_[i]);
    root->append_data(scenes_elm);
    root->append_text("\n");
  }
  
  //write whether scenes contain certain category labels, or are used for training/testing
  //for(unsigned ci=0; ci < categories_.size(); ci++)
//  {
//    bxml_element* category_elm = new bxml_element("category");
//    category_elm->append_text("\n");
//    
//    category_elm->set_attribute("id", ci);
//    category_elm->set_attribute("name", category_names_[ci]);
//    const vcl_vector<unsigned> &s_idx = categories_[ci];
//    category_elm->set_attribute("nscenes", s_idx.size());
//    
//    vcl_stringstream ss;
//    for (unsigned i = 0; i< s_idx.size(); i++) {
//      ss << s_idx[i] << " ";
//    }
//    category_elm->append_text(ss.str());
//    category_elm->append_text("\n");
//    root->append_data(category_elm);
//    root->append_text("\n");
//  }
  
  //write training scenes
  bxml_element* train_elm = new bxml_element("training_scenes");
  train_elm->append_text("\n");
  
  vcl_stringstream ss;
  unsigned ts = 0;
  
  for (unsigned i = 0; i< training_scenes_.size(); i++) {
    if (training_scenes_[i]){
      ss << i << " ";
      ts++;
    }
  }
  
  train_elm->set_attribute("nscenes", ts);
  train_elm->append_text(ss.str());
  train_elm->append_text("\n");
  root->append_data(train_elm);
  root->append_text("\n");
  
  
  
  //write to disk  
  vcl_ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();  
}

