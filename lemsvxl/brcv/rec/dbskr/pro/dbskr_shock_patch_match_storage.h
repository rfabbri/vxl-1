// This is brcv/rec/dbskr/pro/dbskr_shock_patch_match_storage.h
#ifndef dbskr_shock_patch_match_storage_h_
#define dbskr_shock_patch_match_storage_h_
//:
// \file
// \brief Storage class for shock patch matching
// \author Ozge Can Ozcanli
// \date March 21 2007
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_curve_match_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage_sptr.h>

#include <vil/vil_image_resource_sptr.h>

//: Storage class for dbskr_edit_distance
class dbskr_shock_patch_match_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbskr_shock_patch_match_storage();

  //: Destructor
  virtual ~dbskr_shock_patch_match_storage();

  virtual vcl_string type() const { return "shock_patch_match"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbskr_shock_patch_match_storage"; }

  void set_match(dbskr_shock_patch_match_sptr m) { match_ = m; }
  dbskr_shock_patch_match_sptr get_match() { return match_; }
  
  void set_curve_match(dbskr_shock_patch_curve_match_sptr m) { curve_match_ = m; }
  dbskr_shock_patch_curve_match_sptr get_curve_match() { return curve_match_; }
  
  void set_image1(vil_image_resource_sptr img) { img1_ = img; }
  void set_image2(vil_image_resource_sptr img) { img2_ = img; }

  vil_image_resource_sptr get_image1(void) { return img1_; }
  vil_image_resource_sptr get_image2(void) { return img2_; }

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


private:

  dbskr_shock_patch_match_sptr match_;
  dbskr_shock_patch_curve_match_sptr curve_match_;
  vil_image_resource_sptr img1_, img2_;
};

//: Create a smart-pointer to a dbskr_shock_match_storage.
struct dbskr_shock_patch_match_storage_new : public dbskr_shock_patch_match_storage_sptr
{
  typedef dbskr_shock_patch_match_storage_sptr base;

  //: Constructor - creates a default dbskr_shock_patch_storage_sptr.
  dbskr_shock_patch_match_storage_new() : base(new dbskr_shock_patch_match_storage()) { }
};

#endif //dbskr_shock_patch_match_storage_h_
