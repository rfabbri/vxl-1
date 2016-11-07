//This is bvis/bvis_mview_io_dialog.h

#ifndef bvis_mview_io_dialog_h_
#define bvis_mview_io_dialog_h_

//:
// \file
// \brief The bvis_mview_io_dialog 
// \author J.L. Mundy
// \date April 26, 2007
//
// \verbatim
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>

class bvis_mview_io_dialog
{

public:

  ~bvis_mview_io_dialog();

  static bool 
    input_dialog(vcl_vector< vcl_string > const& input_type_list,
                 vcl_vector< vcl_string > & input_names);

  static bool 
    output_dialog(vcl_vector< vcl_string > const& output_type_list,
                  vcl_vector< vcl_string > const& output_suggested_names,
                  vcl_vector< unsigned >& output_view_ids,
                  vcl_vector< vcl_string >& output_names);
protected:

private:
  bvis_mview_io_dialog();//static methods only
};

#endif
