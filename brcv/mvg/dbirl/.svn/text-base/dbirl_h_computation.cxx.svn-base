#include "dbirl_h_computation.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vsol/vsol_point_2d.h>

//-------------------------------------
bool 
dbirl_h_computation::compute_affine( 
  vcl_vector< dbinfo_track_geometry_sptr >& tracks,
  vcl_vector< vgl_h_matrix_2d<double> >& homographies,
  int ground_frame )
{
  // Deduce the number of frames and constraints in the sequence.
  int num_frames = 0; 
  int num_constraints = 0;
  for( int i = 0; i < static_cast<int>(tracks.size()); i++ ){
    if( 1+static_cast<int>(tracks[i]->end_frame()) > num_frames )
      num_frames = 1+tracks[i]->end_frame();
    num_constraints += 2*( tracks[i]->end_frame()-tracks[i]->start_frame() );
  }
  if( num_frames == 0 ) return false;

  // Form the solution matrix. S*V=B
  vnl_matrix<double> S( num_constraints, 6*( num_frames - 1 ), 0 );
  vnl_vector<double> B( num_constraints, 0 );
  vnl_vector<double> V( 6*( num_frames - 1 ), 0 );

  int r = 0;
  for( int i = 0; i < static_cast<int>(tracks.size()); i++ ){
    int start = tracks[i]->start_frame();
    int adj_start = start;
    if( start == ground_frame ) adj_start = -1;
    if( start > ground_frame ) adj_start--;
    vsol_point_2d_sptr start_point = tracks[i]->point( start );

    for( int current = start+1; current <= static_cast<int>(tracks[i]->end_frame()); current++ ){
      int adj_current = current;
      if( current == ground_frame ) adj_current = -1;
      if( current > ground_frame ) adj_current--;
      vsol_point_2d_sptr current_point = tracks[i]->point( current );

      if( adj_start == -1 ){

        B(r) = -start_point->x();
        B(r+1) = -start_point->y();
      }
      else{

        S(r,6*adj_start+0) = start_point->x();
        S(r,6*adj_start+1) = start_point->y();
        S(r,6*adj_start+2) = 1;
        S(r+1,6*adj_start+3) = start_point->x();
        S(r+1,6*adj_start+4) = start_point->y();
        S(r+1,6*adj_start+5) = 1;
      }
      if( adj_current == -1 ){
        B(r) = current_point->x();
        B(r+1) = current_point->y();
      }
      else {
        S(r,6*adj_current+0) = -current_point->x();
        S(r,6*adj_current+1) = -current_point->y();
        S(r,6*adj_current+2) = -1;
        S(r+1,6*adj_current+3) = -current_point->x();
        S(r+1,6*adj_current+4) = -current_point->y();
        S(r+1,6*adj_current+5) = -1;
      }
      r+=2;
    }
  }

  // Solve SV=B
  vnl_svd<double> Ssvd(S);
  V = Ssvd.solve( B );
  homographies.clear();

  // Pull out the homographies
  for( int i = 0; i < num_frames; i++ ){
    vnl_matrix_fixed<double,3,3> h(0.0); h(2,2) = 1;
    if( i < ground_frame ){
      h(0,0)=V(6*i+0); h(0,1)=V(6*i+1); h(0,2)=V(6*i+2);
      h(1,0)=V(6*i+3); h(1,1)=V(6*i+4); h(1,2)=V(6*i+5);
    }
    else if( i > ground_frame ){
      h(0,0)=V(6*(i-1)+0); h(0,1)=V(6*(i-1)+1); h(0,2)=V(6*(i-1)+2);
      h(1,0)=V(6*(i-1)+3); h(1,1)=V(6*(i-1)+4); h(1,2)=V(6*(i-1)+5);
    }
    else // i == ground_frame
      h(0,0) = h(1,1) = 1;
    homographies.push_back( vgl_h_matrix_2d<double>( h ) );
  }
  return true;
}



//-------------------------------------
bool 
dbirl_h_computation::compute_similarity( 
  vcl_vector< dbinfo_track_geometry_sptr >& tracks,
  vcl_vector< vgl_h_matrix_2d<double> >& homographies,
  int ground_frame )
{
  int max_frame = 10000;

  // Deduce the number of frames and constraints in the sequence.
  int num_frames = 0; 
  int num_constraints = 0;
  for( int i = 0; i < static_cast<int>(tracks.size()); i++ ){
    vcl_cerr << "\nTrack " << i << ": " << tracks[i]->start_frame() 
      << " " << tracks[i]->end_frame() << '\n';
    if( 1+static_cast<int>(tracks[i]->end_frame()) > num_frames )
      num_frames = 1+tracks[i]->end_frame();
    int track_end = tracks[i]->end_frame();
    if( track_end > max_frame ) track_end = max_frame;
    num_constraints += 2*( track_end - tracks[i]->start_frame() );
  }
  if( num_frames > max_frame+1 ) num_frames = max_frame+1;
  if( num_frames == 0 ) return false;

  // Form the solution matrix. S*V=B
  vnl_matrix<double> S( num_constraints, 4*( num_frames - 1 ), 0 );
  vnl_vector<double> B( num_constraints, 0 );
  vnl_vector<double> V( 4*( num_frames - 1 ), 0 );

  int r = 0;
  for( int i = 0; i < static_cast<int>(tracks.size()); i++ ){
    int start = tracks[i]->start_frame();
    int adj_start = start;
    if( start == ground_frame ) adj_start = -1;
    if( start > ground_frame ) adj_start--;
    vsol_point_2d_sptr start_point = tracks[i]->point( start );

    for( int current = start+1; current <= static_cast<int>(tracks[i]->end_frame()); current++ ){
      if( current > max_frame ) break;
      int adj_current = current;
      if( current == ground_frame ) adj_current = -1;
      if( current > ground_frame ) adj_current--;
      vsol_point_2d_sptr current_point = tracks[i]->point( current );

      if( adj_start == -1 ){
        B(r) = -start_point->x();
        B(r+1) = -start_point->y();
      }
      else{
        S(r,4*adj_start+0) = start_point->x();
        S(r,4*adj_start+1) = start_point->y();
        S(r,4*adj_start+2) = 1;
        S(r+1,4*adj_start+1) = -start_point->x();
        S(r+1,4*adj_start+0) = start_point->y();
        S(r+1,4*adj_start+3) = 1;
      }
      if( adj_current == -1 ){
        B(r) = current_point->x();
        B(r+1) = current_point->y();
      }
      else {
        S(r,4*adj_current+0) = -current_point->x();
        S(r,4*adj_current+1) = -current_point->y();
        S(r,4*adj_current+2) = -1;
        S(r+1,4*adj_current+1) = current_point->x();
        S(r+1,4*adj_current+0) = -current_point->y();
        S(r+1,4*adj_current+3) = -1;
      }
      r+=2;
    }
  }
/*
  // DEBUGGING START:
  vcl_string bs("c:\\data\\house\\db.txt");
  vcl_ofstream ofp( bs.c_str() );
  if(!ofp){
      vcl_cout << "\n Could not open debugging file.\n";
      return false; }
  ofp << "\nS matrix:\n";
  for( int i = 0; i < S.rows(); i++ ){
    bool start_found = false;
    for( int j = 0; j < S.cols(); j++ ){
      if( S(i,j) != 0 && !start_found ){
        ofp << "row " << i << " from col " << j << ":\n" << S(i,j);
        start_found = true; }
      else if( S(i,j) != 0 && start_found )
        ofp << ' ' << S(i,j);
      else if( S(i,j) == 0 && start_found ){
        ofp << '\n';
        start_found = false; }
    }
  }
  ofp << "\nPoints:\n";
  for( int i = 0; i < num_frames; i++ ){
    ofp << "\nFrame " << i << ":\n";
    for( int t = 0; t < tracks.size(); t++ ){
      if( i >= tracks[t]->start_frame() && i <= tracks[t]->end_frame() ) 
        ofp << tracks[t]->point(i)->x() << " " << tracks[t]->point(i)->y() << '\n';
      else
        ofp << "no point\n";
    }
  }
  // DEBUGGING END
*/
  // Solve SV=B
  vnl_svd<double> Ssvd(S);
  V = Ssvd.solve( B );
  homographies.clear();

  // Pull out the homographies
  for( int i = 0; i < num_frames; i++ ){
    vnl_matrix_fixed<double,3,3> h(0.0); h(2,2) = 1;
    if( i < ground_frame ){
      h(0,0)=V(4*i+0); h(0,1)=V(4*i+1); h(0,2)=V(4*i+2);
      h(1,0)=-V(4*i+1); h(1,1)=V(4*i+0); h(1,2)=V(4*i+3);
    }
    else if( i > ground_frame ){
      h(0,0)=V(4*(i-1)+0); h(0,1)=V(4*(i-1)+1); h(0,2)=V(4*(i-1)+2);
      h(1,0)=-V(4*(i-1)+1); h(1,1)=V(4*(i-1)+0); h(1,2)=V(4*(i-1)+3);
    }
    else // i == ground_frame
      h(0,0) = h(1,1) = 1;
    homographies.push_back( vgl_h_matrix_2d<double>( h ) );
  }
  vnl_matrix_fixed<double,3,3> h2(0.0); h2(0,0) = h2(1,1) = h2(2,2) = 1;
/*
    // DEBUGGING START:
  ofp << "\nHomographies:\n";
  for( int i = 0; i < homographies.size(); i++ ){
    ofp << homographies[i] << "\n\n";
  }
  ofp.close();
  // DEBUGGING END
*/
  return true;
}
