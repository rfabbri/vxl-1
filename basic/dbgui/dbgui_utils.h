// This is breye/dbgui/dbgui_utils.h
#ifndef dbgui_utils_h_
#define dbgui_utils_h_
//:
// \file
// \brief  More useful static functions to augment vgui_utils
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 9/9/04
//
// \verbatim
//  Modifications
//    Mar 27, 2010   Nhon Trinh   Added a non-interactive version of render_to_ps
// \endverbatim

#include <vgui/vgui_tableau_sptr.h>
#include <vcl_string.h>

//: More useful static functions to augment vgui_utils
class dbgui_utils
{
 public:
  //: Render the tableaux hierarchy to PostScript
  static void render_to_ps(const vgui_tableau_sptr& tableau);

  //: Render a tableau hierarchy to a PostScript file
  // Options for output_type: ".ps", ".eps", ".pdf", ".txt" 
  // If file_type not provided, extension of output_file will be used
  static void render_to_ps(const vgui_tableau_sptr& tableau, 
    const vcl_string output_file, const vcl_string& output_type = "");

};

#endif // dbgui_utils_h_
