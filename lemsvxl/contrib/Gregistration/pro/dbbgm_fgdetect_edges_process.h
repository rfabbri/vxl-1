// This is edgemodeling/pro/dbbgm_fgdetect_edges_process.h
#ifndef dbbgm_fgdetect_edges_process_h_
#define dbbgm_fgdetect_edges_process_h_

//:
// \file
// \brief Process that detects foreground using a mixture model
// \author Vishal Jain (vj@lems.brown.edu)
// \date 10/25/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbbgm/dbbgm_image_of.h>

//: Derived video process class for detecting foreground
class dbbgm_fgdetect_edges_process : public bpro1_process {

public:

  dbbgm_fgdetect_edges_process();
  virtual ~dbbgm_fgdetect_edges_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  dbdet_edgemap_sptr fg_detect(dbdet_edgemap_sptr edgemap);


};

#endif
