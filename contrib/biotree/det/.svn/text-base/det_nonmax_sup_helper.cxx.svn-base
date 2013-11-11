#include "det_nonmax_sup_helper.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>

vbl_array_3d<bool> det_nonmax_sup_helper::intersection_flags(
    vgl_vector_3d<double> const& dir, 
    vcl_vector<vgl_point_3d<double> > const &vertice,
    vcl_vector<vbl_array_3d<int> > const & vertice_index)
{

  // 8 vertices
  assert(vertice_index.size() == 8);

  int nx = vertice_index[0].get_row1_count();
  int ny = vertice_index[0].get_row2_count();
  int nz = vertice_index[0].get_row3_count();

  // equations for plane is ax + by +cz = 0
  double a = dir.x(); 
  double b = dir.y();
  double c = dir.z();

  vcl_vector<double> dist(vertice.size());

  for(unsigned int l=0; l < vertice.size(); l++)
  {
    dist[l] = a * vertice[l].x() + 
      b*vertice[l].y() + c*vertice[l].z();
  }

  // for each voxel in the suppression box
  vbl_array_3d<bool> intersect_flags(nx, ny, nz);
  for(int bi = 0; bi < nx; bi++)
    for(int bj = 0; bj < ny; bj++)
      for(int bk = 0; bk < nz; bk++)
      {
        // if no intersect between plane and voxel 
        if((dist[vertice_index[0].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[1].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[2].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[3].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[4].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[5].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[6].operator()(bi,bj,bk)] >= 0 && 
            dist[vertice_index[7].operator()(bi,bj,bk)] >= 0 ) || 
            dist[vertice_index[0].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[1].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[2].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[3].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[4].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[5].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[6].operator()(bi,bj,bk)] <= 0 && 
            dist[vertice_index[7].operator()(bi,bj,bk)] <= 0 )

          intersect_flags[bi][bj][bk] = false;
        else // if intersection
          intersect_flags[bi][bj][bk] = true;

      }

  return intersect_flags;

}

vbl_array_2d<double> det_nonmax_sup_helper::proj_axis_plane(
    det_cylinder_map const & input,
    int i, int j, int k, // position in the input grid
    vgl_vector_3d<double> const& dir,
    vbl_array_3d<bool> const& intersect_flags
    )
{

  int axis = det_nonmax_sup_helper::closest_axis(dir);
  
  int nx = intersect_flags.get_row1_count();
  int ny = intersect_flags.get_row2_count();
  int nz = intersect_flags.get_row3_count();

  assert( i >= (nx-1)/2 && i <input.nx() - (nx-1)/2);
  assert( j >= (ny-1)/2 && j <input.ny() - (ny-1)/2);
  assert( k >= (nz-1)/2 && k <input.nz() - (nz-1)/2);
  
  double a = dir.x();
  double b = dir.y();
  double c = dir.z();
  double factor = vcl_sqrt(a*a + b*b + c*c);

  a /= factor;
  b /= factor;
  c /= factor;

  vbl_array_2d<double> proj;

  switch(axis)
  {
    case 1:

      proj.resize(ny, nz);
      
      for(int bi = 0; bi<ny; bi++)
        for(int bj=0; bj<nz; bj++)
        {
          vcl_vector<int> q; // a vector of intesected voxel indice.
          for(int bk =0; bk < nx; bk++) 
            if(intersect_flags[bk][bi][bj])
              q.push_back(bk);

          double dist_sum = 0;
          for(unsigned int l=0; l < q.size(); l++)
          {
            double distance = a*(q[l] - (nx-1)/2 ) + 
              b*(bi - (ny -1)/2) + c*(bj - (nz - 1)/2);

            dist_sum += vcl_sqrt(3.0)*0.5 - fabs(distance);
          }

          proj[bi][bj] = 0.0;

          for(unsigned int l = 0; l < q.size(); l++)
          {
            double distance = vcl_sqrt(3.0)*0.5 - 
              fabs(a*(q[l] - (nx-1)/2 ) + b*(bi - (ny -1)/2) + 
                  c*(bj - (nz - 1)/2));

            proj[bi][bj] += distance / dist_sum *
              input(i+q[l]-(nx-1)/2, 
                  j+bi-(ny-1)/2, k+bj - (nz-1)/2).strength_;
          }
        }
      

      break;
    
    case 2:
      
      proj.resize(nx, nz);
      
      for(int bi = 0; bi<nx; bi++)
        for(int bj=0; bj<nz; bj++)
        {
          vcl_vector<int> q;
          for(int bk =0; bk < ny; bk++) 
            if(intersect_flags[bi][bk][bj])
              q.push_back(bk);

          double dist_sum = 0;
          for(unsigned int l=0; l < q.size(); l++)
          {
            double distance = a*(bi - (nx-1)/2 ) + 
              b*(q[l] - (ny -1)/2) + c*(bj - (nz - 1)/2);

            dist_sum += vcl_sqrt(3.0)*0.5 - fabs(distance);
          }

          proj[bi][bj] = 0.0;
          
          for(unsigned int l = 0; l < q.size(); l++)
          {
            double distance = vcl_sqrt(3.0)*0.5 - 
              fabs(a*(bi - (nx-1)/2 ) + b*(q[l] - (ny -1)/2) + 
                  c*(bj - (nz - 1)/2));

            proj[bi][bj] += distance / dist_sum * 
              input(i+bi-(nx-1)/2,
                  j+q[l]-(ny-1)/2, k+bj - (nz-1)/2).strength_;
          }
        }


      break;
      
    case 3:
      proj.resize(nx, ny);

      for(int bi = 0; bi<nx; bi++)
        for(int bj=0; bj<ny; bj++)
        {
          vcl_vector<int> q; 
          
          for(int bk =0; bk < nz; bk++) 
            if(intersect_flags[bi][bj][bk])
              q.push_back(bk);

          double dist_sum = 0;
          for(unsigned int l=0; l < q.size(); l++)
          {
            double distance = a*(bi - (nx-1)/2 ) + 
              b*(bj - (ny -1)/2) + c*(q[l] - (nz - 1)/2);

            dist_sum += vcl_sqrt(3.0)*0.5 - fabs(distance);
          }

          proj[bi][bj] = 0.0;
          for(unsigned int l = 0; l < q.size(); l++)
          {
            double distance = vcl_sqrt(3.0)*0.5 - 
              fabs(a*(bi - (nx-1)/2 ) + b*(bj - (ny -1)/2) + 
                  c*(q[l] - (nz - 1)/2));

            proj[bi][bj] += distance / dist_sum * 
              input(i+bi-(nx-1)/2, j+bj - (ny-1)/2, 
                  k+q[l]-(nz-1)/2).strength_;
          }


        }


      break;

    default:
      vcl_cerr << "woophs, weird thing happened\n";

  }

      return proj;

}

int det_nonmax_sup_helper::closest_axis(vgl_vector_3d<double> const & dir)
{          
  double a = dir.x();

  double b = dir.y();

  double c = dir.z();

  int axis = 0;
  if(vcl_fabs(a) >= vcl_fabs(b) && vcl_fabs(a) >= vcl_fabs(c))
    axis = 1; // projection along x-axis
  else
    if(vcl_fabs(b) >= vcl_fabs(a) && vcl_fabs(b) >= vcl_fabs(c))
      axis = 2; // y axis
    else
      axis = 3; // z axis

  return axis;


}

