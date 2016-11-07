// This is brl/vidpro/process/vidpro_kl_process.h
#ifndef vidpro_kltrack_process_h_
#define vidpro_kltrack_process_h_

//:
// \file
// \brief Process that computes kl features and tracks them
// \author Vishal Jain(vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <vidpro/vidpro_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgel/vgel_kl_params.h>
#include <vgel/vgel_kl.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>
#include <vgel/vgel_multi_view_data.h>

//: Derived video process class for computing frame difference
class vidpro_kltrack_process : public vidpro_process {

public:

  vidpro_kltrack_process();
  ~vidpro_kltrack_process();

  vcl_string name();

  //: Clone the process
  virtual vidpro_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
    vgel_kl_params kl_params;
    vgel_kl * kl_points;

};

#endif
