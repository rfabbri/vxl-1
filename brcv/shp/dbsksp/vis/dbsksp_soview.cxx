//:
// \file
#include "dbsksp_soview.h"


//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview::
print(vcl_ostream& os) const
{
  vgui_soview::print(os);
  return os;
}
