// This is contrib/ozge/face_gui/pro/dbru_facedb_storage.cxx

//:
// \file

#include "dbru_facedb_storage.h"
#include <ozge/face_gui/dbru_facedb.h>

//: Constructor
dbru_facedb_storage::dbru_facedb_storage() : n_rows_(25), n_cols_(50)
{
  facedb_ = new dbru_facedb();
}

//: Destructor
dbru_facedb_storage::~dbru_facedb_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbru_facedb_storage::clone() const
{
  return new dbru_facedb_storage(*this);
}

//: get the number of subject stored in facedb
unsigned int dbru_facedb_storage::get_facedb_size(void) {
  return facedb_->n_subjects();
}

//: get a particular subject
vcl_vector<vil_image_resource_sptr>& dbru_facedb_storage::get_subject(unsigned i) { 
  return facedb_->get_subject(i); 
}

//: get a particular face of a particular subject in facedb
vil_image_resource_sptr dbru_facedb_storage::get_face(unsigned i, unsigned j) { 
  return facedb_->get_face(i, j);
}

//: Return IO version number;
short dbru_facedb_storage::version() const
{
  return 1;
}

//: Binary save self to stream.
void dbru_facedb_storage::b_write(vsl_b_ostream &os) const
{
  bool flag = true;
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  if(facedb_)
    {
      vsl_b_write(os, flag);
      //vsl_b_write(os, facedb_);
      facedb_->b_write(os);
    }
  else
    vsl_b_write(os, !flag);
}

//: Binary load self from stream.
void dbru_facedb_storage::b_read(vsl_b_istream &is)
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
      //vsl_b_read(is, facedb_);
      facedb_->b_read(is);
    break;
  }
  default:
    vcl_cerr << "I/O ERROR: dbinfo_facedb_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
  
}

