// This is bof_info.h
#ifndef bof_info_h
#define bof_info_h

//:
// \file
// \brief A class to hold info necessary for bag of features 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  1-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene_base.h>


class bof_info
{
  
public:
  //: Constructor - from xml info file
  bof_info(vcl_string bof_dir);
  
  //: Constructor - from global_pca or global_taylor xml info file and number of means. It also writes this class' info file
  bof_info(vcl_string global_dir, vcl_string bof_dir);
  
  //: Accesors that load the scenes
  boxm_scene_base_sptr load_feature_scene (int scene_id);
  boxm_scene_base_sptr load_valid_scene (int scene_id);
  boxm_scene_base_sptr load_category_scene (int scene_id);
  boxm_scene_base_sptr load_cluster_id_scene (int scene_id);

  vcl_string aux_dir(int scene_id) { return aux_dirs_[scene_id]; }
  
  void init_category_scenes(char label);  
  
  void xml_write();
  
  unsigned nscenes() {return scenes_.size(); }
   
  friend class bof_codebook_utils;
  friend class bof_scene_categories;
  friend class bof_util;
  template <unsigned dim> friend class bof_class_codebook_util;
  
  
  friend void bof_init_scene_categories_xml(vcl_string bof_dir);
protected:
  
  //: A vector to hold feature scene paths
  vcl_vector<vcl_string> scenes_;
  //: A vector to hold paths to keep any kind of auxiliary scene or info(must be in the same order as scenes_)
  //  Examples are auxiliary scene indicating which cells are used for training, testing, or object categories
  vcl_vector<vcl_string> aux_dirs_;
  //: A vector to hold the finest cell lenght of the corresponding scene scene
  vcl_vector<double> finest_cell_length_;

  
  //: A vector to hold list of scenes ids(orderd as scenes_) that contain such categories
  //vcl_vector < vcl_vector<unsigned> > categories_;
  
  //: A vector to hold category names. The order is equivalent to that of categories_
  //vcl_vector <vcl_string > category_names_;
  
  //: A vector that indicates whether a scene should be used as train (True) or test(False). Order is equivalent to scenes_
  vcl_vector<bool> training_scenes_;
  
  //: Number of means
  //unsigned K_;
  
  //: Main directory, where bof_info.xml file is stored
  vcl_string path_out_;
  
  vcl_string xml_path() { return path_out_ + "/bof_info.xml"; }

};
#endif
