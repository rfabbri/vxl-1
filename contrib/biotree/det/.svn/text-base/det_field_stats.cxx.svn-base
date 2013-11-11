#include "det_field_stats.h"

det_field_stats::det_field_stats()
{
}

det_field_stats::det_field_stats(vcl_vector< xmvg_filter_response<double> > responses) : 
responses_(responses)
{
}

void det_field_stats::compute_responses_at(xscan_dummy_scan scan, vgl_box_3d<double> box)
{
  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);
  // resolution accuracy
  double resolution = 0.1;
  // filter variables
  double f_radius = 0.0125;
  double f_length = 0.2;
  // filter position and orientation
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  // construct descriptor
  xmvg_no_noise_filter_descriptor fdx(f_radius, f_length, f_centre, f_orientation_x);
  xmvg_no_noise_filter_descriptor fdy(f_radius, f_length, f_centre, f_orientation_y);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filter
  xmvg_no_noise_filter_3d fx(fdx);
  xmvg_no_noise_filter_3d fy(fdy);
  xmvg_no_noise_filter_3d fz(fdz);
  // push the constructed filters into a vector
  vcl_vector<xmvg_no_noise_filter_3d> filters;
  filters.push_back(fx);
  filters.push_back(fy);
  filters.push_back(fz);
  // construct the composite filter
  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);
  // splat resource
  bioproc_dummy_splat_resource<double, xmvg_no_noise_filter_3d> splr(scan, comp3d);
  // process object
  bioproc_filtering_proc<double, xmvg_no_noise_filter_3d> proc(scan_ref, box, resolution, comp3d);

  // filtering
  proc.execute();

  responses_ = proc.responses();
  this->write_responses("C:/MatlabR14/work/vxl.mat");
}

void det_field_stats::write_responses(char *filename)
{
  int num_points = responses_.size();
  int num_filters = responses_[0].size();
  vnl_matrix<double> result(num_filters,num_points);
  // fill result matrix with response values
  for(int i=0;i<num_filters;i++)
    for(int j=0;j<num_points;j++)
      result[i][j] = responses_[j].get(i);
  // write this as a matlab file
  vnl_matlab_filewrite matlab(filename);
  matlab.write(result,"result");
}
