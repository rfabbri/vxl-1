// This is dbsks/dbsks_io_shapelet_grid.cxx

//:
// \file

#include "dbsks_io_shapelet_grid.h"
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_math.h>


// ============================================================================
// shapelet_grid_params
// ============================================================================

//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const dbsks_shapelet_grid_params& params)
{
  // 1. save version number
  const short version = 2;
  vsl_b_write(os, version);

  // 2. save the parameters
  vsl_b_write(os, params.center_x);
  vsl_b_write(os, params.center_y);
  vsl_b_write(os, params.step_x);
  vsl_b_write(os, params.step_y);
  vsl_b_write(os, params.half_num_x);
  vsl_b_write(os, params.half_num_y);
  
  vsl_b_write(os, params.num_psi);

  vsl_b_write(os, params.half_range_phiA);
  vsl_b_write(os, params.half_num_phiA);

  vsl_b_write(os, params.half_range_phiB);
  vsl_b_write(os, params.half_num_phiB);

  vsl_b_write(os, params.max_m);
  vsl_b_write(os, params.half_num_m);

  vsl_b_write(os, params.max_log2_len);
  vsl_b_write(os, params.half_num_len);

  vsl_b_write(os, params.max_log2_rA);
  vsl_b_write(os, params.half_num_rA);
  vsl_b_write(os, params.force_phi_centered_at_pi_over_2);
}


//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, dbsks_shapelet_grid_params& params)
{
  if (!is) return;

  // read version number
  short version;
  vsl_b_read(is, version);

  switch (version)
  {
  case 1:
    // read back the same order they were written
    vsl_b_read(is, params.center_x);
    vsl_b_read(is, params.center_y);
    vsl_b_read(is, params.step_x);
    vsl_b_read(is, params.step_y);
    vsl_b_read(is, params.half_num_x);
    vsl_b_read(is, params.half_num_y);
    
    vsl_b_read(is, params.num_psi);

    float max_phiA;
    vsl_b_read(is, max_phiA);
    params.half_range_phiA = max_phiA - float(vnl_math::pi_over_2);
    vsl_b_read(is, params.half_num_phiA);

    float max_phiB;
    vsl_b_read(is, max_phiB);
    params.half_range_phiB = max_phiB - float(vnl_math::pi_over_2);


    vsl_b_read(is, params.half_num_phiB);

    vsl_b_read(is, params.max_m);
    vsl_b_read(is, params.half_num_m);

    vsl_b_read(is, params.max_log2_len);
    vsl_b_read(is, params.half_num_len);

    vsl_b_read(is, params.max_log2_rA);
    vsl_b_read(is, params.half_num_rA);

    params.force_phi_centered_at_pi_over_2 = true;
    break;

  case 2:
    // read back the same order they were written
    vsl_b_read(is, params.center_x);
    vsl_b_read(is, params.center_y);
    vsl_b_read(is, params.step_x);
    vsl_b_read(is, params.step_y);
    vsl_b_read(is, params.half_num_x);
    vsl_b_read(is, params.half_num_y);
    
    vsl_b_read(is, params.num_psi);

    vsl_b_read(is, params.half_range_phiA);
    vsl_b_read(is, params.half_num_phiA);

    vsl_b_read(is, params.half_range_phiB);
    vsl_b_read(is, params.half_num_phiB);

    vsl_b_read(is, params.max_m);
    vsl_b_read(is, params.half_num_m);

    vsl_b_read(is, params.max_log2_len);
    vsl_b_read(is, params.half_num_len);

    vsl_b_read(is, params.max_log2_rA);
    vsl_b_read(is, params.half_num_rA);
    vsl_b_read(is, params.force_phi_centered_at_pi_over_2);
    
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const dbsks_shapelet_grid& arc_grid) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  return;
}






// ============================================================================
// shapelet_grid
// ============================================================================

//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const dbsks_shapelet_grid& grid)
{
  // 1. save version number
  const short version = 1;
  vsl_b_write(os, version);

  // 2. save the data in the grid
  
  // x
  vsl_b_write(os, grid.min_xA_);
  vsl_b_write(os, grid.max_xA_);
  vsl_b_write(os, grid.num_xA_);
  vsl_b_write(os, grid.step_xA_);
  vsl_b_write(os, grid.xA_);

  // y
  vsl_b_write(os, grid.min_yA_);
  vsl_b_write(os, grid.max_yA_);
  vsl_b_write(os, grid.num_yA_);
  vsl_b_write(os, grid.step_yA_);
  vsl_b_write(os, grid.yA_);

  // psiA - it should cover all angles [0, 2pi]
  vsl_b_write(os, grid.min_psiA_);
  vsl_b_write(os, grid.max_psiA_);
  vsl_b_write(os, grid.num_psiA_);
  vsl_b_write(os, grid.step_psiA_);
  vsl_b_write(os, grid.psiA_);
  

  // phiA
  vsl_b_write(os, grid.min_phiA_);
  vsl_b_write(os, grid.max_phiA_);
  vsl_b_write(os, grid.num_phiA_);
  vsl_b_write(os, grid.step_phiA_);
  vsl_b_write(os, grid.phiA_);

  // phiB
  vsl_b_write(os, grid.min_phiB_);
  vsl_b_write(os, grid.max_phiB_);
  vsl_b_write(os, grid.num_phiB_);
  vsl_b_write(os, grid.step_phiB_);
  vsl_b_write(os, grid.phiB_);


  // m
  vsl_b_write(os, grid.min_m_);
  vsl_b_write(os, grid.max_m_);
  vsl_b_write(os, grid.num_m_);
  vsl_b_write(os, grid.step_m_);
  vsl_b_write(os, grid.m_);


  // rA
  vsl_b_write(os, grid.ref_rA_);
  vsl_b_write(os, grid.min_log2_rA_);
  vsl_b_write(os, grid.max_log2_rA_);
  vsl_b_write(os, grid.num_rA_);
  vsl_b_write(os, grid.step_log2_rA_);
  vsl_b_write(os, grid.rA_);

  // len
  vsl_b_write(os, grid.ref_len_);
  vsl_b_write(os, grid.min_log2_len_);
  vsl_b_write(os, grid.max_log2_len_);
  vsl_b_write(os, grid.num_len_);
  vsl_b_write(os, grid.step_log2_len_);
  vsl_b_write(os, grid.len_);

  // has_front_arc
  vsl_b_write(os, grid.has_front_arc_);

  // has_rear_arc
  vsl_b_write(os, grid.has_rear_arc_);
  
  return;
}



//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, dbsks_shapelet_grid& grid)
{  
  if (!is) return;

  // read version number
  short version;
  vsl_b_read(is, version);

  switch (version)
  {
  case 1:
    // x
    vsl_b_read(is, grid.min_xA_);
    vsl_b_read(is, grid.max_xA_);
    vsl_b_read(is, grid.num_xA_);
    vsl_b_read(is, grid.step_xA_);
    vsl_b_read(is, grid.xA_);

    // y
    vsl_b_read(is, grid.min_yA_);
    vsl_b_read(is, grid.max_yA_);
    vsl_b_read(is, grid.num_yA_);
    vsl_b_read(is, grid.step_yA_);
    vsl_b_read(is, grid.yA_);

    // psiA - it should cover all angles [0, 2pi]
    vsl_b_read(is, grid.min_psiA_);
    vsl_b_read(is, grid.max_psiA_);
    vsl_b_read(is, grid.num_psiA_);
    vsl_b_read(is, grid.step_psiA_);
    vsl_b_read(is, grid.psiA_);
    

    // phiA
    vsl_b_read(is, grid.min_phiA_);
    vsl_b_read(is, grid.max_phiA_);
    vsl_b_read(is, grid.num_phiA_);
    vsl_b_read(is, grid.step_phiA_);
    vsl_b_read(is, grid.phiA_);

    // phiB
    vsl_b_read(is, grid.min_phiB_);
    vsl_b_read(is, grid.max_phiB_);
    vsl_b_read(is, grid.num_phiB_);
    vsl_b_read(is, grid.step_phiB_);
    vsl_b_read(is, grid.phiB_);


    // m
    vsl_b_read(is, grid.min_m_);
    vsl_b_read(is, grid.max_m_);
    vsl_b_read(is, grid.num_m_);
    vsl_b_read(is, grid.step_m_);
    vsl_b_read(is, grid.m_);


    // rA
    vsl_b_read(is, grid.ref_rA_);
    vsl_b_read(is, grid.min_log2_rA_);
    vsl_b_read(is, grid.max_log2_rA_);
    vsl_b_read(is, grid.num_rA_);
    vsl_b_read(is, grid.step_log2_rA_);
    vsl_b_read(is, grid.rA_);

    // len
    vsl_b_read(is, grid.ref_len_);
    vsl_b_read(is, grid.min_log2_len_);
    vsl_b_read(is, grid.max_log2_len_);
    vsl_b_read(is, grid.num_len_);
    vsl_b_read(is, grid.step_log2_len_);
    vsl_b_read(is, grid.len_);

    // has_front_arc
    vsl_b_read(is, grid.has_front_arc_);

    // has_rear_arc
    vsl_b_read(is, grid.has_rear_arc_);
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const dbsks_shapelet_grid& arc_grid) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  return;
}




