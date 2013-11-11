// This is rpl/rrel/rrel_homography2d_est_epipole.cxx
#include "rrel_homography2d_est_epipole.h"

#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

rrel_homography2d_est_epipole :: rrel_homography2d_est_epipole( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                                                const vcl_vector< vgl_homg_point_2d<double> > & to_pts, 
                                                vgl_homg_point_2d<double> from_epi, vgl_homg_point_2d<double> to_epi)
  : rrel_homography2d_est( from_pts, to_pts, 6)
{
    
    from_epi_[0] = from_epi.x();
    from_epi_[1] = from_epi.y();
    from_epi_[2] = from_epi.w();
    assert( from_epi_[2] != 0 );
    
    to_epi_[0] = to_epi.x();
    to_epi_[1] = to_epi.y();
    to_epi_[2] = to_epi.w();
    assert( to_epi_[2] != 0 );
    
  
}

rrel_homography2d_est_epipole :: rrel_homography2d_est_epipole( const vcl_vector< vnl_vector<double> > & from_pts,
                                                const vcl_vector< vnl_vector<double> > & to_pts ,
                                                vnl_vector<double> from_epi, vnl_vector<double> to_epi)
                                                 
  : rrel_homography2d_est( from_pts, to_pts, 6)
{
     from_epi_= from_epi;
 to_epi_= to_epi;
  
}

rrel_homography2d_est_epipole::~rrel_homography2d_est_epipole()
{
}




bool
rrel_homography2d_est_epipole :: fit_from_minimal_set( const vcl_vector<int>& point_indices,
                                               vnl_vector<double>& params ) const
{
  vnl_matrix< double > A(9, 9, 0.0);
  assert( point_indices.size() == min_num_pts_ );

  for ( unsigned int i=0; i<min_num_pts_; ++i )
  {
    int loc = point_indices[ i ];
    A( 2*i, 0 ) = A( 2*i+1, 3 ) = from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 1 ) = A( 2*i+1, 4 ) = from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 2 ) = A( 2*i+1, 5 ) = from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 6 ) = -1 * from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 0 ];
    A( 2*i, 7 ) = -1 * from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 0 ];
    A( 2*i, 8 ) = -1 * from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 0 ];
    A( 2*i+1, 6 ) = -1 * from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 1 ];
    A( 2*i+1, 7 ) = -1 * from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 1 ];
    A( 2*i+1, 8 ) = -1 * from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 1 ];
  }
  A( 2*min_num_pts_, 0) = A( 2*min_num_pts_+1, 3 ) =from_epi_[0]* to_epi_[2];
  A( 2*min_num_pts_, 1) = A( 2*min_num_pts_+1, 4 ) =from_epi_[1]* to_epi_[2];
  A( 2*min_num_pts_, 2) = A( 2*min_num_pts_+1, 5 ) =from_epi_[2]* to_epi_[2];
  A( 2*min_num_pts_, 6 ) = -1 * from_epi_[0] * to_epi_[0];
  A( 2*min_num_pts_, 7 ) = -1 * from_epi_[1] * to_epi_[0];
  A( 2*min_num_pts_, 8 ) = -1 * from_epi_[2] * to_epi_[0];
  A( 2*min_num_pts_+1, 6 ) = -1 * from_epi_[0] * to_epi_[1];
  A( 2*min_num_pts_+1, 7 ) = -1 * from_epi_[1] * to_epi_[1];
  A( 2*min_num_pts_+1, 8 ) = -1 * from_epi_[2] * to_epi_[1];
  vnl_svd<double> svd( A, 1.0e-8 );

  if ( svd.rank() < homog_dof_ ) {
    return false;    // singular fit
  }
  else {
    params = svd.nullvector();
    return true;
  }
}





