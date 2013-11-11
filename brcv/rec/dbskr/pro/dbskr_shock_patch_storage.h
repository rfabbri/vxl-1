// This is brcv/rec/dbskr/pro/dbskr_shock_patch_storage.h
#ifndef dbskr_shock_patch_storage_h_
#define dbskr_shock_patch_storage_h_
//:
// \file
// \brief Storage class for shock patches
// \author Ozge Can Ozcanli
// \date March 13 2007
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbskr/dbskr_shock_patch_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>

#include <vil/vil_rgb.h>
#include <vil/vil_image_resource_sptr.h>

//: Storage class for dbskr_edit_distance
class dbskr_shock_patch_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbskr_shock_patch_storage();

  //: Destructor
  virtual ~dbskr_shock_patch_storage();

  virtual vcl_string type() const { return "shock_patch"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbskr_shock_patch_storage"; }

  unsigned size() { return patches_.size(); }

  //: set the shock matching correspondence
  void add_patch(dbskr_shock_patch_sptr sp); 

  //: get the patch
  dbskr_shock_patch_sptr get_patch(unsigned i) { return patches_[i]; }

  //: get the shock matching correspondence
  vcl_vector<dbskr_shock_patch_sptr>& get_patches(void) { return patches_; }
  vcl_map<int, dbskr_shock_patch_sptr>& get_map(void) { return id_sptr_map_; }

  bool load_patch_shocks(vcl_string storage_name, vcl_string st_postfix );
  bool load_patch_shocks_and_create_trees(vcl_string storage_name, vcl_string st_postfix, bool elastic_splice_cost, bool construct_circular_ends, bool combined_edit,
                                    float scurve_sample_ds, float scurve_interp_ds);

  void set_tree_params_for_matching(bool elastic_splice_cost, bool construct_circular_ends, bool combined_edit,
                                    float scurve_sample_ds, float scurve_interp_ds);

  bool create_ps_images(vil_image_resource_sptr background_img, vcl_string filename_base, bool outer_poly, vil_rgb<int>& color);

  
  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  void clear();

private:

  vcl_vector<dbskr_shock_patch_sptr> patches_;
  vcl_map<int, dbskr_shock_patch_sptr> id_sptr_map_;
};

//: Create a smart-pointer to a dbskr_shock_match_storage.
struct dbskr_shock_patch_storage_new : public dbskr_shock_patch_storage_sptr
{
  typedef dbskr_shock_patch_storage_sptr base;

  //: Constructor - creates a default dbskr_shock_patch_storage_sptr.
  dbskr_shock_patch_storage_new() : base(new dbskr_shock_patch_storage()) { }
};

#endif //dbskr_shock_patch_storage_h_
