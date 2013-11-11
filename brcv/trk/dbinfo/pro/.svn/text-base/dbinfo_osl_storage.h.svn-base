// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_osl_storage.h
#ifndef dbinfo_osl_storage_h_
#define dbinfo_osl_storage_h_

//:
// \file
// \brief The bpro1 storage class for the object signature library
// \author J.L. Mundy
// \date May 10, 2006
//
//
// \verbatim
//  Modifications
// \endverbatim
#include <vcl_cassert.h>
#include <bpro1/bpro1_storage.h>
#include <dbinfo/dbinfo_osl_sptr.h>

class dbinfo_osl_storage : public bpro1_storage {

public:

  dbinfo_osl_storage();
  virtual ~dbinfo_osl_storage();
  virtual vcl_string type() const { return "dbinfo_osl_storage"; }
  

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
  virtual vcl_string is_a() const { return "dbinfo_osl_storage"; }


  //:set osl
  void set_osl(dbinfo_osl_sptr const& osl){osl_ = osl;}
  
  //:retrieve the osl
  dbinfo_osl_sptr osl(){return osl_;}

 protected:

  dbinfo_osl_sptr osl_;
  
};

//Inlined functions
inline void vsl_b_read(vsl_b_istream &is, dbinfo_osl_storage & osl_st)
{osl_st.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_osl_storage & osl_st)
{osl_st.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_osl_storage* osl_st)
{
  //Don't allow writing for null feature data
  assert(osl_st);
  osl_st->b_write(os);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_osl_storage* &osl_st)
{
  delete osl_st;
  osl_st = new dbinfo_osl_storage();
  osl_st->b_read(is);
}
#include <dbinfo/pro/dbinfo_osl_storage_sptr.h>

//: Create a smart-pointer to a dbinfo_osl_storage.
struct dbinfo_osl_storage_new : public dbinfo_osl_storage_sptr
{
  typedef dbinfo_osl_storage_sptr base;

  //: Constructor - creates a default dbinfo_osl_storage_sptr.
  dbinfo_osl_storage_new() : base(new dbinfo_osl_storage()) { }
};

#endif // dbinfo_osl_storage_h_
