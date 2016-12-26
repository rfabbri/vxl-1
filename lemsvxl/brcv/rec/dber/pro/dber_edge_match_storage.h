// This is brcv/rec/dber/pro/dber_edge_match_storage.h
#ifndef dber_edge_match_storage_h_
#define dber_edge_match_storage_h_
//:
// \file
// \brief Storage class for edge-based matching and recognition
// \author Ozge Can Ozcanli
// \date Oct 23 2006
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>

#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_line_2d.h>

#include <dber/pro/dber_edge_match_storage_sptr.h>

//: Storage class for dbskr_edit_distance
class dber_edge_match_storage : public bpro1_storage 
{
public:

  //: Constructor
  dber_edge_match_storage();

  //: Destructor
  virtual ~dber_edge_match_storage();

  virtual vcl_string type() const { return "edge_match"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dber_edge_match_storage"; }

  //: set the assignment
  void set_assignement(vcl_vector<unsigned>& assign) { assign_ = assign; }

  //: get the assignment
  vcl_vector<unsigned>& get_assignment(void) { return assign_; }

  //: set lines
  void set_lines1(vcl_vector<vsol_line_2d_sptr>& l) { lines1_ = l; }
  void set_lines2(vcl_vector<vsol_line_2d_sptr>& l) { lines2_ = l; }

  //: get lines
  vcl_vector<vsol_line_2d_sptr>& get_lines1(void) { return lines1_; }
  vcl_vector<vsol_line_2d_sptr>& get_lines2(void) { return lines2_; }

  void set_dominant_dir1(vgl_line_2d<double> dir) { dominant_dir1_ = dir; }
  void set_dominant_dir2(vgl_line_2d<double> dir) { dominant_dir2_ = dir; }

  vgl_line_2d<double> get_dominant_dir1(void) { return dominant_dir1_; }
  vgl_line_2d<double> get_dominant_dir2(void) { return dominant_dir2_; }

private:

  vcl_vector<vsol_line_2d_sptr> lines1_, lines2_;
  vcl_vector<unsigned> assign_;

  vgl_line_2d<double> dominant_dir1_, dominant_dir2_;
};

//: Create a smart-pointer to a dber_edge_match_storage.
struct dber_edge_match_storage_new : public dber_edge_match_storage_sptr
{
  typedef dber_edge_match_storage_sptr base;

  //: Constructor - creates a default dber_edge_match_storage_sptr.
  dber_edge_match_storage_new() : base(new dber_edge_match_storage()) { }
};

#endif //dber_edge_match_storage_h_
