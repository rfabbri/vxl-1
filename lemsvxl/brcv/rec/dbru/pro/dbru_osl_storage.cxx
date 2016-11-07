// This is brcv/rec/dbru/pro/dbru_osl_storage.cxx

//:
// \file

#include "dbru_osl_storage.h"
//#include <dbru/dbru_object.h>
#include <dbru/dbru_osl.h>

//: Constructor
dbru_osl_storage::dbru_osl_storage() : n_rows_(25), n_cols_(50)
{
  osl_ = new dbru_osl();
}

//: Destructor
dbru_osl_storage::~dbru_osl_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbru_osl_storage::clone() const
{
  return new dbru_osl_storage(*this);
}

//: get the number of objects stored in OSL
unsigned int dbru_osl_storage::get_osl_size(void) {
  return osl_->n_objects();
}

//: get a particular object
dbru_object_sptr dbru_osl_storage::get_object(unsigned i) { 
  return osl_->get_object(i); 
}

//: get a particular polygon of a particular object in osl
vsol_polygon_2d_sptr dbru_osl_storage::get_polygon(unsigned i, unsigned j) { 
  return osl_->get_polygon(i, j);
}
//: get a particular observations of a particular object in osl
dbinfo_observation_sptr dbru_osl_storage::get_observation(unsigned i, unsigned j) { 
  return osl_->get_prototype(i, j);
}

//: Return IO version number;
short dbru_osl_storage::version() const
{
  return 1;
}

//: Binary save self to stream.
void dbru_osl_storage::b_write(vsl_b_ostream &os) const
{
  bool flag = true;
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  if(osl_)
    {
      vsl_b_write(os, flag);
      //vsl_b_write(os, osl_);
      osl_->b_write(os);
    }
  else
    vsl_b_write(os, !flag);
}

//: Binary load self from stream.
void dbru_osl_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    bool flag = false;
    vsl_b_read(is, flag);
    if(flag)
      //vsl_b_read(is, osl_);
      osl_->b_read(is);
    break;
  }
  default:
    vcl_cerr << "I/O ERROR: dbinfo_osl_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
  
}

