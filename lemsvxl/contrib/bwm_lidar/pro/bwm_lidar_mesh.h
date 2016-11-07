//:
// \file
// \brief 
//
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date 11/29/2007
//      
// \endverbatim

#ifndef bwm_lidar_mesh_process_h_
#define bwm_lidar_mesh_process_h_

#include "all_includes.h"

///#include "flimap_reader.h"
///#include "bwm_lidar_algo.h"
///#include "set_operations.h"

class bwm_lidar_mesh_process : public bpro1_process
{
public:
  bwm_lidar_mesh_process();

  virtual ~bwm_lidar_mesh_process();

  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  /*void save_to_wrl(vcl_string inp_file_path, vnl_matrix<double>& points_colors);*/

protected:
  vcl_string input_path;
};

#endif
