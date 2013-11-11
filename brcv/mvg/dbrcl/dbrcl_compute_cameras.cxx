#ifndef _dbrcl_compute_cameras_cxx_
#define _dbrcl_compute_cameras_cxx_

#include "dbrcl_compute_cameras.h"

#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>
#include <vsol/vsol_point_2d.h>

//----------------------------------------------
bool 
dbrcl_compute_cameras::compute_cameras(
  vgel_multi_view_data_vertex_sptr& tracked_points,
  const vcl_vector< vpgl_proj_camera<double> >& known_cameras,
  const vcl_vector<int>& known_frames,
  vcl_vector< vpgl_proj_camera<double> >& cameras )
{
  vcl_cerr << "dbrcl_compute_cameras::compute_cameras:\n" 
    << " computing all cameras\n";
  if( known_cameras.size() < 2 ){
    vcl_cerr << "  insufficient constrained cameras to compute the other cameras\n";
    return false;
  }

  int num_frames = tracked_points->get_nb_views();
  for( int f = 0; f < num_frames; f++ ){

    // Check if we already know this camera.
    bool know_camera = false;
    for( int i = 0; i < static_cast<int>(known_frames.size()); i++ )
      if( known_frames[i] == f ){
        cameras.push_back( known_cameras[i] );
        know_camera = true;
        if( verbose )
          vcl_cerr << "  Frame " << f << " camera:\n"
            << known_cameras[i].get_matrix();
      }
    if( know_camera ) continue;

    // Otherwise, get all correspondences between this frame and known frames.
    vcl_vector< vcl_vector< vtol_vertex_2d_sptr > > pf, pi, pj;
    vcl_vector< vpgl_proj_camera<double> > observed_cameras_i, observed_cameras_j;
    int num_constraints = 0;
    for( int i = 0; i < static_cast<int>(known_frames.size()); i++ ){
      for( int j = i+1; j < static_cast<int>(known_frames.size()); j++ ){
        vcl_vector< vtol_vertex_2d_sptr > npf, npi, npj;
        tracked_points->get( known_frames[i], f, known_frames[j], npi, npf, npj );
        if( npf.size() >= 10 ){
          num_constraints += npf.size();
          pf.push_back( npf ); pi.push_back( npi ); pj.push_back( npj );
          observed_cameras_i.push_back( known_cameras[i] );
          observed_cameras_j.push_back( known_cameras[j] );
        }
      }
    }
    if( pf.size() == 0 ){
      cameras.push_back( vpgl_proj_camera<double>() );
      vcl_cerr << "  insufficient tracks to compute camera from frame " << f << '\n';
      continue;
    }

    // Do a brute force search for the optimal parameter.
    double start_window_radius = 10;
    double search_increments = 20;
    vnl_vector<double> best_params(2);
    best_params(0) = known_cameras[0].get_matrix()(0,2);
    best_params(1) = known_cameras[0].get_matrix()(1,2);
    double best_error = 100000;

    for( int lvl = 0; lvl < 14; lvl++ ){

      double current_radius = start_window_radius/vcl_pow(2,double(lvl));
      vnl_vector<double> old_best_params = best_params;
      for( double dx = -current_radius; dx < current_radius; 
        dx += current_radius/search_increments ){
        for( double dy = -current_radius; dy < current_radius; 
          dy += current_radius/search_increments ){
          vnl_vector<double> these_params = old_best_params;
          these_params(0) = these_params(0)+dx;
          these_params(1) = these_params(1)+dy;

          // Compute the error from these parameters.
          vcl_vector<double> this_error;
          for( int k = 0; k < static_cast<int>(pf.size()); k++ ){
            vnl_matrix_fixed<double,3,4> camera1 =
              observed_cameras_i[ k ].get_matrix();
            vnl_matrix_fixed<double,3,4> camera2 =
              observed_cameras_j[ k ].get_matrix();
            double dbx1 = these_params(0) - camera1(0,2);
            double dby1 = these_params(1) - camera1(1,2);
            double dbx2 = these_params(0) - camera2(0,2);
            double dby2 = these_params(1) - camera2(1,2);
            for( int i = 0; i < static_cast<int>(pf[k].size()); i++ ){
              double dx1 = pf[k][i]->x() - pi[k][i]->x();
              double dy1 = pf[k][i]->y() - pi[k][i]->y();
              double dx2 = pf[k][i]->x() - pj[k][i]->x();
              double dy2 = pf[k][i]->y() - pj[k][i]->y();
              if( dx1*dx1+dy1*dy1+dx2*dx2+dy2*dy2 < .0001 ) continue;
              double scale = (dx1*dbx1+dy1*dby1+dx2*dbx2+dy2*dby2)/(dx1*dx1+dy1*dy1+dx2*dx2+dy2*dy2);
              double ex1 = dbx1 - scale*dx1;
              double ey1 = dby1 - scale*dy1;
              double ex2 = dbx2 - scale*dx2;
              double ey2 = dby2 - scale*dy2;
              this_error.push_back( ex1*ex1 + ey1*ey1 + ex2*ex2 + ey2*ey2 );
            }
          }

          vcl_sort( this_error.begin(), this_error.end() );
          double final_error = this_error[ this_error.size()/2 ]; //double floor(double)
          if( final_error < best_error ){
            best_error = final_error;
            best_params = these_params;
          }
        }
      }
    }

    vnl_matrix_fixed<double,3,4> this_camera = cameras[0].get_matrix();
    this_camera(0,2) = best_params(0);
    this_camera(1,2) = best_params(1);
    if( verbose )
      vcl_cerr << "  Frame " << f << " camera:\n" << this_camera;
    cameras.push_back( vpgl_proj_camera<double>( this_camera ) );
  }

  vcl_cerr << " success\n";
  return true;
}


//----------------------------------------------
bool 
dbrcl_compute_cameras::compute_cameras(
  vcl_vector< dbinfo_track_geometry_sptr >& tracks,
  const vcl_vector< vpgl_proj_camera<double> >& known_cameras,
  const vcl_vector<int>& known_frames,
  vcl_vector< vpgl_proj_camera<double> >& cameras )
{
  vcl_cerr << "dbrcl_compute_cameras::compute_cameras:\n" 
    << " computing all cameras\n";
  if( known_cameras.size() < 2 ){
    vcl_cerr << "  insufficient constrained cameras to compute the other cameras\n";
    return false;
  }

  int num_frames = 0; 
  for( int i = 0; i < static_cast<int>(tracks.size()); i++ ){
    if( 1+static_cast<int>(tracks[i]->end_frame()) > num_frames )
      num_frames = 1+tracks[i]->end_frame();
  }
  if( num_frames == 0 ) return false;

  for( int f = 0; f < num_frames; f++ ){

    // Check if we already know this camera.
    bool know_camera = false;
    for( int i = 0; i < static_cast<int>(known_frames.size()); i++ )
      if( known_frames[i] == f ){
        cameras.push_back( known_cameras[i] );
        know_camera = true;
        if( verbose )
          vcl_cerr << "  Frame " << f << " camera:\n"
            << known_cameras[i].get_matrix();
      }
    if( know_camera ) continue;

    // Otherwise, form a solution matrix.
    vnl_matrix<double> S( 2*known_frames.size()*tracks.size(), 2+tracks.size(), 0 );
    vnl_vector<double> B( 2*known_frames.size()*tracks.size(), 0 );
    vnl_vector<double> V( 2+tracks.size(), 0 ); // THESE ARE WRONG
    for( int t = 0; t < static_cast<int>(tracks.size()); t++ ){
      for( int k = 0; k < static_cast<int>(known_frames.size()); k++ ){
        if( static_cast<int>(tracks[t]->start_frame()) > known_frames[k] || 
            static_cast<int>(tracks[t]->end_frame()) < known_frames[k] ) continue;
        S( 2*(t*known_frames.size()+k)+0, 0 ) = 1;
        S( 2*(t*known_frames.size()+k)+1, 1 ) = 1;
        S( 2*(t*known_frames.size()+k)+0, 2+t ) = 
          tracks[t]->point( known_frames[k] )->x() - tracks[t]->point( f )->x();
        S( 2*(t*known_frames.size()+k)+1, 2+t ) = 
          tracks[t]->point( known_frames[k] )->y() - tracks[t]->point( f )->y();
        B( 2*(t*known_frames.size()+k)+0 ) = known_cameras[k].get_matrix()(0,2);
        B( 2*(t*known_frames.size()+k)+1 ) = known_cameras[k].get_matrix()(1,2);
      }
    }
    vnl_svd<double> Ssvd( S );
    V = Ssvd.solve( B );

    vnl_matrix_fixed<double,3,4> this_camera = cameras[0].get_matrix();
    this_camera(0,2) = V(0);
    this_camera(1,2) = V(1);
    if( verbose )
      vcl_cerr << "  Frame " << f << " camera:\n" << this_camera;
    cameras.push_back( vpgl_proj_camera<double>( this_camera ) );
  }

  vcl_cerr << " success\n";
  return true;
}


#endif // _dbrcl_compute_cameras_cxx_
