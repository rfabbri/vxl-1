// This is brcv/rec/dbru/pro/dbru_facedb_storage.h
#ifndef dbru_facedb_storage_h_
#define dbru_facedb_storage_h_
//:
// \file
// \brief Storage class for facedb, 
// \author Ozge Can Ozcanli
// \date Aug 19, 06
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>

#include <vil/vil_image_resource_sptr.h>
#include <ozge/face_gui/pro/dbru_facedb_storage_sptr.h>
#include <ozge/face_gui/dbru_facedb_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <ozge/face_gui/dbru_facedb_sptr.h>
#include <vcl_cassert.h>

//: Storage class for dbskr_edit_distance
class dbru_facedb_storage : public bpro1_storage 
{
public:
  //: Constructor
  dbru_facedb_storage();

  //: Destructor
  virtual ~dbru_facedb_storage();

  virtual vcl_string type() const { return "dbru_facedb_storage"; }

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbru_facedb_storage"; }

  //: set the facedb
  void set_facedb(dbru_facedb_sptr facedb) { facedb_ = facedb; }

  //: get the facedb
  dbru_facedb_sptr facedb(void) { return facedb_; }

  //: get the number of subjects stored in facedb
  unsigned int get_facedb_size(void);

  //: get a particular subject
  vcl_vector<vil_image_resource_sptr>& get_subject(unsigned i);

  //: get a particular image of a particular object in facedb
  vil_image_resource_sptr get_face(unsigned i, unsigned j);
  
  // for visualization
  unsigned get_nrows() { return n_rows_; }
  unsigned get_ncols() { return n_cols_; }
  void set_nrows(unsigned n) { n_rows_ = n; }
  void set_ncols(unsigned n) { n_cols_ = n; }

private:

  dbru_facedb_sptr facedb_;
  unsigned n_rows_, n_cols_;  // for visualization of the facedb
};

//Inlined functions
inline void vsl_b_read(vsl_b_istream &is, dbru_facedb_storage & facedb_st)
{facedb_st.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_facedb_storage & facedb_st)
{facedb_st.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_facedb_storage* facedb_st)
{
  //Don't allow writing for null feature data
  assert(facedb_st);
  facedb_st->b_write(os);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbru_facedb_storage* &facedb_st)
{
  delete facedb_st;
  facedb_st = new dbru_facedb_storage();
  facedb_st->b_read(is);
}
#include <ozge/face_gui/pro/dbru_facedb_storage_sptr.h>

//: Create a smart-pointer to a dbru_facedb_storage.
struct dbru_facedb_storage_new : public dbru_facedb_storage_sptr
{
  typedef dbru_facedb_storage_sptr base;

  //: Constructor - creates a default dbru_facedb_storage_sptr.
  dbru_facedb_storage_new() : base(new dbru_facedb_storage()) { }
};

#endif //dbru_facedb_storage_h_

