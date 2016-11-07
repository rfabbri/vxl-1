// This is brcv/rec/dbru/pro/dbru_osl_storage.h
#ifndef dbru_osl_storage_h_
#define dbru_osl_storage_h_
//:
// \file
// \brief Storage class for OSL, OSL: object signature library is nothing but an array of dbru_objects
//        each object has labeled tracked polygons from a video sequence
//        objects include observations for each of their frames so loading video is not necessary anymore
// \author Ozge Can Ozcanli
// \date Dec 15 2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>

#include <dbru/pro/dbru_osl_storage_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_osl_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vbl/vbl_array_1d.h>

//: Storage class for dbskr_edit_distance
class dbru_osl_storage : public bpro1_storage 
{
public:
  //: Constructor
  dbru_osl_storage();

  //: Destructor
  virtual ~dbru_osl_storage();

  virtual vcl_string type() const { return "dbru_osl_storage"; }

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
  virtual vcl_string is_a() const { return "dbru_osl_storage"; }

  //: set the OSL
  void set_osl(dbru_osl_sptr osl) { osl_ = osl; }

  //: get the OSL
  dbru_osl_sptr osl(void) { return osl_; }

  //: get the number of objects stored in OSL
  unsigned int get_osl_size(void);

  //: get a particular object
  dbru_object_sptr get_object(unsigned i);

  //: get a particular polygon of a particular object in osl
  vsol_polygon_2d_sptr get_polygon(unsigned i, unsigned j);
  //: get a particular observations of a particular object in osl
  dbinfo_observation_sptr get_observation(unsigned i, unsigned j);

  // for visualization
  unsigned get_nrows() { return n_rows_; }
  unsigned get_ncols() { return n_cols_; }
  void set_nrows(unsigned n) { n_rows_ = n; }
  void set_ncols(unsigned n) { n_cols_ = n; }

private:

  dbru_osl_sptr osl_;

  // for visualization
  unsigned n_rows_, n_cols_;  // for visualization of the OSL
  // a map for each object that gives which polygon is displayed in which column
  vcl_vector< vcl_map<int, int> > column_map_;
};

//Inlined functions
inline void vsl_b_read(vsl_b_istream &is, dbru_osl_storage & osl_st)
{osl_st.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_osl_storage & osl_st)
{osl_st.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_osl_storage* osl_st)
{
  //Don't allow writing for null feature data
  assert(osl_st);
  osl_st->b_write(os);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbru_osl_storage* &osl_st)
{
  delete osl_st;
  osl_st = new dbru_osl_storage();
  osl_st->b_read(is);
}
#include <dbru/pro/dbru_osl_storage_sptr.h>

//: Create a smart-pointer to a dbru_osl_storage.
struct dbru_osl_storage_new : public dbru_osl_storage_sptr
{
  typedef dbru_osl_storage_sptr base;

  //: Constructor - creates a default dbru_osl_storage_sptr.
  dbru_osl_storage_new() : base(new dbru_osl_storage()) { }
};

#endif //dbru_osl_storage_h_

