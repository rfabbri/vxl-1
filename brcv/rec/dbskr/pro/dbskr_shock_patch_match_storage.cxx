// This is brcv/rec/dbskr/pro/dbskr_shock_patch_match_storage.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>

//: Constructor
dbskr_shock_patch_match_storage::dbskr_shock_patch_match_storage() 
{
}

//: Destructor
dbskr_shock_patch_match_storage::~dbskr_shock_patch_match_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbskr_shock_patch_match_storage::clone() const
{
  return new dbskr_shock_patch_match_storage(*this);
}

//: Binary save self to stream.
void dbskr_shock_patch_match_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  match_->b_write(os);

  return;
}

//: Binary load self from stream.
void dbskr_shock_patch_match_storage::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        match_ = new dbskr_shock_patch_match();
        match_->b_read(is);
        break;
      }
  }
}


