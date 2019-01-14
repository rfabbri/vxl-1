//:
// \file
#include "dbsksp_soview.h"


//: Print details about this vsol object to the given stream.
std::ostream& dbsksp_soview::
print(std::ostream& os) const
{
  vgui_soview::print(os);
  return os;
}
