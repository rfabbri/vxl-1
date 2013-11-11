#include "det_nonmaxium_suppression.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/algo/vnl_qr.h>
#include <vbl/vbl_array_2d.h>
#include "det_nonmax_sup_helper.h"

det_nonmaxium_suppression::det_nonmaxium_suppression(int n)
  : nv_(n+1)
{
  // assert only odd number of voxels are in the suppression box
  assert( n%2 != 0 );

  // set up the vertice

  vertice_.resize(nv_*nv_*nv_);

  for(int i=0; i<nv_; i++)
    for(int j=0; j<nv_; j++)
      for(int k=0; k<nv_; k++)
        vertice_[vertice_index(i, j, k)].set(-double(n)/2.0+i, 
            -double(n)/2.0+j, -double(n)/2.0+k);

  // setup the vertice map
  for(int i=0; i<8; i++)
    vertice_map_.push_back(vbl_array_3d<int>(n,n,n));
  
  for(int i = 0; i< n; i++)
    for(int j=0; j<n; j++)
      for(int k=0; k<n; k++)
      {
        // push 8 vertice index into the map
        vertice_map_[0].operator()(i,j,k) = vertice_index(i, j, k);
        vertice_map_[1].operator()(i,j,k) = vertice_index(i, j, k+1);
        vertice_map_[2].operator()(i,j,k) = vertice_index(i, j+1, k);
        vertice_map_[3].operator()(i,j,k) = vertice_index(i+1, j, k);
        vertice_map_[4].operator()(i,j,k) = vertice_index(i+1, j+1, k+1);
        vertice_map_[5].operator()(i,j,k) = vertice_index(i+1, j+1, k);
        vertice_map_[6].operator()(i,j,k) = vertice_index(i+1, j, k+1);
        vertice_map_[7].operator()(i,j,k) = vertice_index(i, j+1, k+1);
      }
}

void det_nonmaxium_suppression::create_vertices(int n, 
                                           vcl_vector<vbl_array_3d<int> > &vertice_map,
                                           vcl_vector<vgl_point_3d<double> > &vertice)
{
  // assert only odd number of voxels are in the suppression box
  assert( n%2 != 0 );
  nv_ = n+1;

  vertice.resize(nv_*nv_*nv_);

  for(int i=0; i<nv_; i++)
    for(int j=0; j<nv_; j++)
      for(int k=0; k<nv_; k++) {
        int index = vertice_index(i, j, k);
        vertice[index].set(-double(n)/2.0+i, 
            -double(n)/2.0+j, -double(n)/2.0+k);
      }

  // setup the vertice map
  for(int i=0; i<8; i++)
    vertice_map.push_back(vbl_array_3d<int>(n,n,n));
  
  for(int i = 0; i< n; i++) {
    for(int j=0; j<n; j++) {
      for(int k=0; k<n; k++)
      {
        // push 8 vertice index into the map
        vertice_map[0].operator()(i,j,k) = vertice_index(i, j, k);
        vertice_map[1].operator()(i,j,k) = vertice_index(i, j, k+1);
        vertice_map[2].operator()(i,j,k) = vertice_index(i, j+1, k);
        vertice_map[3].operator()(i,j,k) = vertice_index(i+1, j, k);
        vertice_map[4].operator()(i,j,k) = vertice_index(i+1, j+1, k+1);
        vertice_map[5].operator()(i,j,k) = vertice_index(i+1, j+1, k);
        vertice_map[6].operator()(i,j,k) = vertice_index(i+1, j, k+1);
        vertice_map[7].operator()(i,j,k) = vertice_index(i, j+1, k+1);
      }
    }
  }

}
det_cylinder_map det_nonmaxium_suppression::apply(det_cylinder_map const& input, 
    double threshold)
{
  // for each point in the cylinder map
  int n = nv_ - 1; // suppression box with n neighbors
  int margin = (n-1)/2;

  int nx = input.nx();
  int ny = input.ny();
  int nz = input.nz();

  // store the resulting cylinder map
  det_cylinder_map output(nx, ny, nz);
  
  for(int i=margin; i<nx-margin; i++)
    for(int j=margin; j<ny-margin; j++)
      for(int k=margin; k<nz-margin; k++)
      {
        //
        // 1) mark all the voxels which has intersection with cutting plane 
        //  
        vgl_vector_3d<double> dir = input(i, j, k).dir_;
        
        vbl_array_3d<bool> intersect_flags = 
          det_nonmax_sup_helper::intersection_flags(dir, vertice_, vertice_map_);
        //
        // 2) project the cutting plane onto a axis aligned plane
        //

        if(input[i][j][k].strength_ <= threshold)
        {
          output(i,j,k).strength_ = 0;
          output(i,j,k).location_ = vgl_point_3d<double> (0., 0., 0.);
          output(i,j,k).dir_ = vgl_vector_3d<double>(0., 0., 0.);
        }
        else{
          vbl_array_2d<double> proj = det_nonmax_sup_helper::proj_axis_plane( input,
              i, j, k,  dir, intersect_flags);
          //
          // 3) find the location of maxium
          //
          if(is_maxium(proj))
          {
            vnl_double_2 subpixel = quadratic_interp_loc(proj);
#if 0
            if(vcl_sqrt(subpixel[0]*subpixel[0]+subpixel[1]*subpixel[1])>1)
            {
              //output(i,j,k).strength_ = input(i,j,k).strength_;
              output(i,j,k).strength_ = 0;
              output(i,j,k).dir_ = vgl_vector_3d<double>(0.,0.,0.); 
              output(i,j,k).location_ = vgl_point_3d<double>(0., 0., 0.);
        
            } 
            else
            {
#endif
              //
              // 4) project back cutting plane
              //

              double a = dir.x();
              double b = dir.y();
              double c = dir.z();

              int axis = det_nonmax_sup_helper::closest_axis(dir);
              switch(axis)
              {
                case 1:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(-(b*subpixel[0]+c*subpixel[1])/a, 
                        subpixel[0], subpixel[1]);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                case 2:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(subpixel[0], 
                        -(subpixel[0]*a+subpixel[1]*c)/b, subpixel[1]);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                case 3:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(subpixel[0], subpixel[1], 
                        -(subpixel[0]*a+subpixel[1]*b)/c);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                default:
                  vcl_cerr<< "woophs, wrong axis\n";
              }
#if 0
            }
#endif
          }
          else
          {
            output(i,j,k).strength_ = 0;
            output(i,j,k).location_ = vgl_point_3d<double>(0., 0., 0.);
            output(i,j,k).dir_ = vgl_vector_3d<double>(0., 0., 0.);
            output(i,j,k).radius_ = 0;
          }
        }


      }

  return output;
}

det_cylinder_map det_nonmaxium_suppression::apply_with_radius(det_cylinder_map const& input,
                                                              vbl_array_3d<double> radius_map,
                                                              double threshold)
{
  // for each point in the cylinder map
  int n; // suppression box with n neighbors

  int nx = input.nx();
  int ny = input.ny();
  int nz = input.nz();

  // store the resulting cylinder map
  det_cylinder_map output(nx, ny, nz);
  
  for(int k=0; k<nz; k++)
    for(int j=0; j<ny; j++)
      for(int i=0; i<nx; i++)
      {
        //
        // 1) mark all the voxels which has intersection with cutting plane 
        //  
        vgl_vector_3d<double> dir = input(i, j, k).dir_;
        ///////////////////// GAmze
        int radius = radius_map(i, j, k);
        n = 2.0 * vcl_floor(radius/vcl_sqrt(2.0))+1;
        nv_ = n+1;
        int margin = (n-1)/2; 
        if ((radius == 0) || 
          (i < margin) || (j < margin) || (k < margin) ||
          (i >= nx - margin) || (j >= ny - margin) || (k >= nz - margin)) {
          output(i,j,k).strength_ = 0;
          output(i,j,k).location_ = vgl_point_3d<double> (0., 0., 0.);
          output(i,j,k).dir_ = vgl_vector_3d<double>(0., 0., 0.);
          continue;
        } 

        vcl_vector<vbl_array_3d<int> > vertice_map;
        vcl_vector<vgl_point_3d<double> > vertice;
        create_vertices(n, vertice_map, vertice);
        //vcl_cout << vcl_endl << "[" << i << "," << j << "," << k << "]" <<  vcl_endl;
 
        vbl_array_3d<bool> intersect_flags = 
          det_nonmax_sup_helper::intersection_flags(dir, vertice, vertice_map);
        ///////////////////////////////////
        //
        // 2) project the cutting plane onto a axis aligned plane
        //

        if(input[i][j][k].strength_ <= threshold)
        {
          output(i,j,k).strength_ = 0;
          output(i,j,k).location_ = vgl_point_3d<double> (0., 0., 0.);
          output(i,j,k).dir_ = vgl_vector_3d<double>(0., 0., 0.);
        }
        else{
          vbl_array_2d<double> proj = det_nonmax_sup_helper::proj_axis_plane( input,
              i, j, k,  dir, intersect_flags);
          //
          // 3) find the location of maxium
          //
          if(is_maxium(proj))
          {
            vnl_double_2 subpixel = quadratic_interp_loc(proj);
#if 0
            if(vcl_sqrt(subpixel[0]*subpixel[0]+subpixel[1]*subpixel[1])>1)
            {
              //output(i,j,k).strength_ = input(i,j,k).strength_;
              output(i,j,k).strength_ = 0;
              output(i,j,k).dir_ = vgl_vector_3d<double>(0.,0.,0.); 
              output(i,j,k).location_ = vgl_point_3d<double>(0., 0., 0.);
        
            } 
            else
            {
#endif
              //
              // 4) project back cutting plane
              //

              double a = dir.x();

              double b = dir.y();

              double c = dir.z();

              int axis = det_nonmax_sup_helper::closest_axis(dir);
              switch(axis)
              {
                case 1:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(-(b*subpixel[0]+c*subpixel[1])/a, 
                        subpixel[0], subpixel[1]);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                case 2:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(subpixel[0], 
                        -(subpixel[0]*a+subpixel[1]*c)/b, subpixel[1]);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                case 3:
                  output(i,j,k).location_ = 
                    vgl_point_3d<double>(subpixel[0], subpixel[1], 
                        -(subpixel[0]*a+subpixel[1]*b)/c);
                  output(i,j,k).strength_ = input(i,j,k).strength_;
                  output(i,j,k).dir_ = input(i,j,k).dir_;
                  output(i,j,k).radius_ = input(i,j,k).radius_;
                  break;

                default:
                  vcl_cerr<< "woophs, wrong axis\n";
              }
#if 0
            }
#endif
          }
          else
          {
            output(i,j,k).strength_ = 0;
            output(i,j,k).location_ = vgl_point_3d<double>(0., 0., 0.);
            output(i,j,k).dir_ = vgl_vector_3d<double>(0., 0., 0.);
            output(i,j,k).radius_ = 0;
          }
        }


      }

  return output;
}
bool det_nonmaxium_suppression::is_maxium(const vbl_array_2d<double>& a) const
{
  int nx = a.cols();
  int ny = a.rows();

  assert(nx%2 != 0);
  assert(ny%2 != 0);
  
  double max_strength = a[(nx-1)/2][(ny-1)/2];
  
  for(int i=0; i<ny; i++)
    for(int j=0; j<nx; j++)
      if(a[i][j] > max_strength)
        return false;
  
  return true;
}

vnl_double_2 det_nonmaxium_suppression::
quadratic_interp_loc(const vbl_array_2d<double> &a2d) const
{
  int nx = a2d.cols();
  int ny = a2d.rows();
 
  double mean = 0;
  for(int i=0; i<nx; i++)
    for(int j=0; j<ny; j++)
      mean += a2d[i][j];
  mean /= nx*ny;
 
  assert(nx % 2 != 0);
  assert(ny % 2 != 0);

  int num_eqns = nx*ny;

  int num_vars = 6;
  
  vnl_matrix<double> A(num_eqns, num_vars);
  vnl_vector<double> v(num_eqns);
  
  // set up equations matrix
  for(int i = 0; i < nx; i++)
    for(int j = 0; j < ny; j++)
  {
  
    int x = i - (nx-1)/2;
    int y = j - (ny-1)/2;
    
    v[i*ny + j] = a2d[i][j] - mean;

    A[i*ny + j][0] = x*x;
    A[i*ny + j][1] = y*y;
    A[i*ny + j][2] = x*y;
    A[i*ny + j][3] = x;
    A[i*ny + j][4] = y;
    A[i*ny + j][5] = 1;
  }

  // solve A x = v to find coefficient
  vnl_qr<double> qr(A);
  vnl_vector<double> coef = qr.solve(v);

  // find the ture location by 
  double a = coef[0];
  double b = coef[1];
  double c = coef[2];
  double d = coef[3];
  double e = coef[4];
  double f = coef[5];

  vnl_double_2 pos;

  pos[0] = (c*e-2*b*d)/(4*a*b - c*c);
  pos[1] = (c*d-2*a*e)/(4*a*b - c*c);

  if(fabs(pos[0]) > nx/2 || fabs(pos[1]) > ny/2) 
  {
      pos[0] = 0.;

      pos[1] = 0;
  }

  return pos;
}
