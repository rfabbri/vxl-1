// This is dborl/algo/dborl_xio_image_description.h

#ifndef dborl_xio_image_description_h_
#define dborl_xio_image_description_h_

//:
// \file
// \brief C-like functions to read and write XML files of image description
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 5, 2008
// \verbatim
//   Modifications
// \endverbatim

// see dborl/algo/tests/ --> for example xml files

#include <vcl_string.h>
#include <dborl/dborl_image_description_sptr.h>

//: Read XML file of an image description
bool x_read(const vcl_string& filename, dborl_image_description_sptr& image_desc);


#endif  // _DBORL_IMAGE_DESC_PARSER_H
