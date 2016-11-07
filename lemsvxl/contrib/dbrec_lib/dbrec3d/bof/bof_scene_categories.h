// This is bof_scene_categories.h
#ifndef bof_scene_categories_h
#define bof_scene_categories_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  6-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bof/bof_codebook.h>
#include <bof/bof_labels_keypoint_joint.h>
#include <boxm/boxm_scene.h>
#include <vbl/vbl_ref_count.h>

template <unsigned dim>
class bof_class_codebook_util;

class bof_labels_keypoint_joint;

struct bof_scene_object {
  vcl_string ply_path;
  char class_id;
};

class bof_scene_categories : public vbl_ref_count
{
  
public:
  //: Constructor from directory where bof_info.xml is stored. This class'  xml will also be stored there
  bof_scene_categories(vcl_string bof_dir);
  
  void xml_write();

  //: Loads an object bounding box from .ply file
  vgl_box_3d<double> load_object_bbox();
  
  //: Load all objects for the given scene
  void load_objects(unsigned scene_id, vcl_vector<vgl_box_3d<double> > &bboxes);
  
  //: Label voxels of this scene inside the object with appropiate category
  void label_objects(unsigned scene_id);
  
  //: Learn the joint P(X,C) voxels of this scene inside the object with appropiate category
  bool learn_categories(bof_codebook_sptr codebook, unsigned scene_id, bof_labels_keypoint_joint &p_cx, vcl_string path_out);
   
  //: Given an object in a scene and the joint P(X,C), choose the maximum aposteriory category.
  void classify(bof_codebook_sptr codebook, unsigned scene_id, const bof_labels_keypoint_joint &p_cx, vcl_string classification_dir);
  
  static bool load_bbox_from_ply(const vcl_string &ply_file, vgl_box_3d<double> &box);
  
  static bool scale_ply(const vcl_string &ply_file_in, const vcl_string &ply_file_out, const double scale );
  
  //: Return the number of categories associated with this class
  unsigned nclasses() {return category_names_.size();}
  
  //: Return the number of scenes associated with this class
  unsigned nscenes() {return ply_paths_.size();}
  
  //: Compute representative bounding box for each category
  void bounding_box_statictics();
  
  template<unsigned dim> friend class bof_class_codebook_util;
  
protected:
  
  //: A vector of maps to hold the categories present in a scene.
  //  The map holds a category id (which is equivalet to the id in info_.categories_) and the path to the .ply file
  //  The size and order of the vecto is equivalent to info_.scenes_
  vcl_vector<vcl_vector<bof_scene_object> > ply_paths_;
  
  //: A vector to hold category names. The order is equivalent to that of categories_
  vcl_vector <vcl_string > category_names_;
  
  //: Each category has a vector of scenes associated with it
  vcl_vector<vcl_set<int> > category_scenes_;
  
  //: Bag of fetures info. e.g. scenes to process, number of means.
  bof_info info_;
  
  //: Main directory, where bof_info.xml file is stored
  vcl_string path_out_;
  
  vcl_string xml_path() { return path_out_ + "/bof_category_info.xml"; }
  
  const double bbox_scale_;

};

typedef vbl_smart_ptr<bof_scene_categories> bof_scene_categories_sptr;


//: Function for debugging. Note that the labeled scene puts 255 whenever a voxel belongs to an object
//  Category labels may not be known so only use this function for debugging purposes
void bof_examine_ground_truth(boxm_scene<boct_tree<short, char> > *labeled_scene,
                              double finest_cell_length,
                              const vcl_vector<vcl_string> &ply_paths,
                              const vcl_string &class_name);

void bof_init_scene_categories_xml(vcl_string bof_dir);


/**************** Binary IO for bof_scene_categories *********************/

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bof_scene_categories const &v);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bof_scene_categories &v);


void vsl_print_summary(vcl_ostream &os, const bof_scene_categories &v);


void vsl_b_read(vsl_b_istream& is, bof_scene_categories* v);


void vsl_b_write(vsl_b_ostream& os, const bof_scene_categories* &v);

void vsl_print_summary(vcl_ostream& os, const bof_scene_categories* &v);



#endif
