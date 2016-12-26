//:
// \file

#include "dborl_xio_image_description.h"
#include <dborl/algo/dborl_image_desc_parser.h>


// -----------------------------------------------------------------------------
//: Read XML file of an image description
bool x_read(const vcl_string& filename, dborl_image_description_sptr& image_desc)
{
  dborl_image_desc_parser parser;
  image_desc = dborl_image_description_parse(filename, parser);
  return image_desc;
}
