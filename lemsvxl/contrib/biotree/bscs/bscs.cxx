// This is contrib/biotree/bscs/bscs.cxx
#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_length.h>
#include <vcsl/vcsl_millimeter.h>
#include "bscs.h"

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
bscs::bscs()
  : right_handed_(true)
{
  vcsl_axis_sptr a=new vcsl_axis(vcsl_length::instance().ptr(),vcsl_millimeter::instance().ptr());
  axes_.push_back(a);
  a=new vcsl_axis(*a);
  axes_.push_back(a);
  a=new vcsl_axis(*a);
  axes_.push_back(a);
}
