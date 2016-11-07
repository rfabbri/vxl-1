/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: dbbgm_bgmodel_edges_process.h
//  Date: Mon Oct 28 16:01:53 EDT 2005
***************************************************************/

#ifndef dbbgm_bgmodel_edges_process_header
#define dbbgm_bgmodel_edges_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <dbbgm/dbbgm_image_sptr.h>

#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>

class dbbgm_bgmodel_edges_process : public bpro1_process {

public:

  dbbgm_bgmodel_edges_process();
  ~dbbgm_bgmodel_edges_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
   dbbgm_image_sptr model_;
private:

};

#endif
