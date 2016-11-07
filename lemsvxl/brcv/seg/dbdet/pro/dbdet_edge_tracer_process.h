// This is brcv/seg/dbdet/pro/dbdet_edge_tracer_process.h
#ifndef dbdet_edge_tracer_process_h_
#define dbdet_edge_tracer_process_h_

//:
// \file
// \brief A process to trace edgel pixels from images to output geometric curves
// \author Amir Tamrakar
// \date 04/03/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters.h>


#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

//: This process traces through the edge pixels in an image and outputs image contours
class dbdet_edge_tracer_process : public bpro_process 
{
public:

  dbdet_edge_tracer_process();
  virtual ~dbdet_edge_tracer_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  //: extract image contours from the edge image
  void trace_edges(vil_image_view<vxl_byte> & image, unsigned edge_val, 
                   vcl_vector<vsol_spatial_object_2d_sptr> & contours);

  void trace_contours_from_end_points();

  // additional functions
  bool Is_Inside_Image (int row, int col);
  bool Is_8_Neighbor (int x1, int y1, int x2, int y2);
  bool Is_4_Neighbor (int x1, int y1, int x2, int y2);
  int Num_of_8_Neighbor (int row, int col);
  int Find_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col);
  int Grow_Seed_Edge(int x, int y, dbdet_edgel_chain* cur_chain)
  
  void trace_contours_from_end_points();
};

#endif
