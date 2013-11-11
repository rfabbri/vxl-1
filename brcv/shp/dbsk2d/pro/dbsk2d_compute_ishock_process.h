// This is brcv/shp/dbsk2d/pro/dbsk2d_compute_ishock_process.h
#ifndef dbsk2d_compute_ishock_process_h_
#define dbsk2d_compute_ishock_process_h_
//:
// \file
// \brief Vpro process for computing intrinsic shocks from vsol objects
// \author Mark Johnson
// \date Aug 28 2003
//
// \verbatim
//  Modifications
//
//  Ozge C. Ozcanli - Jan 2, 07 - if the boundary has many collinearities, this result in many degeneracies
//                                which are not handled by the current shock computation algorithm
//                                add noise in an attempt to remove these collinearities
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

class dbsk2d_compute_ishock_process : public bpro1_process 
{
public:

  dbsk2d_compute_ishock_process();
  virtual ~dbsk2d_compute_ishock_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  vsol_polygon_2d_sptr smooth_closed_contour(vsol_polygon_2d_sptr polygon);

  vsol_polyline_2d_sptr add_noise_to_contour(vsol_polyline_2d_sptr poly, double noise_radius);
  vsol_polygon_2d_sptr add_noise_to_contour(vsol_polygon_2d_sptr poly, double noise_radius);
  vsol_polyline_2d_sptr fit_lines_to_contour(vsol_polyline_2d_sptr poly, double rms);
  vsol_polygon_2d_sptr fit_lines_to_contour(vsol_polygon_2d_sptr poly, double rms);
  
protected:

private:

};

#endif //dbsk2d_compute_ishock_process_h_
