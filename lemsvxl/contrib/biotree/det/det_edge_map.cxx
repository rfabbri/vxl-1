#include "det_edge_map.h"
#include <det/det_nonmaxium_suppression.h>
#include <det/det_edge_nonmaximum_suppression.h>
#include <det/det_cylinder_detect.h>

det_edge_map::det_edge_map(unsigned nx, unsigned ny, unsigned nz)
  : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
}

det_edge_map::det_edge_map(vbl_array_3d<det_map_entry> const & map)
  : map_(map)
{
  nx_ = map.get_row1_count();
  ny_ = map.get_row2_count();
  nz_ = map.get_row3_count();
}

void det_edge_map::resize(int nx, int ny, int nz)
{
  map_.resize(nx, ny, nz);

  nx_ = nx;
  ny_ = ny;
  nz_ = nz;
}

det_edge_map::det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                           vcl_vector<xmvg_filter_response<double> > const& responses_x, 
                           vcl_vector<xmvg_filter_response<double> > const& responses_y, 
                           vcl_vector<xmvg_filter_response<double> > const& responses_z,
                           double sharpening_coefficient)
                           : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
  int index = 0;
  for(unsigned int k=0; k < nz; k++)
    for(unsigned int j=0; j < ny; j++)
      for(unsigned int i=0; i < nx; i++)
      {
        xmvg_filter_response<double> resp_x(responses_x[index]);
        xmvg_filter_response<double> resp_y(responses_y[index]);
        xmvg_filter_response<double> resp_z(responses_z[index++]);
        vgl_vector_3d<double> direction(resp_x.get(0)*sharpening_coefficient, resp_y.get(0)*sharpening_coefficient, resp_z.get(0));
        det_map_entry entry;
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        entry.strength_ = direction.length();
        map_[i][j][k] = entry;
      }
}
det_edge_map::det_edge_map(const vbl_array_3d<double>& gradient_x, 
                           const vbl_array_3d<double>& gradient_y, 
                           const vbl_array_3d<double>& gradient_z)
                           : map_(gradient_x.get_row1_count(), gradient_x.get_row2_count(), gradient_x.get_row3_count()), 
                           nx_(gradient_x.get_row1_count()), ny_(gradient_x.get_row2_count()), nz_(gradient_x.get_row3_count())
{
  for(unsigned int k=0; k < nz_; k++)
    for(unsigned int j=0; j < ny_; j++)
      for(unsigned int i=0; i < nx_; i++)
      {
        vgl_vector_3d<double> direction( gradient_x[i][j][k], gradient_y[i][j][k], gradient_z[i][j][k]);
        det_map_entry entry;
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        entry.strength_ = direction.length();
        map_[i][j][k] = entry;
      }
}

det_edge_map::det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                           const double *gradient_x, 
                           const double *gradient_y, 
                           const double *gradient_z)
                           : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
  const double* cur_x = gradient_x;
  const double* cur_y = gradient_y;
  const double* cur_z = gradient_z;
  for(unsigned int k=0; k < nz; k++)
  {
    for(unsigned int j=0; j < ny; j++)
    {
      for(unsigned int i=0; i < nx; i++)
      {
        vgl_vector_3d<double> direction(*cur_x, *cur_y, *cur_z);
        det_map_entry entry;
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        entry.strength_ = direction.length();
        map_[i][j][k] = entry;

        cur_x++;
        cur_y++;
        cur_z++;
      }
    }
  }
}

det_edge_map::det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                           const float *gradient_x, 
                           const float *gradient_y, 
                           const float *gradient_z)
                           : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
  const float* cur_x = gradient_x;
  const float* cur_y = gradient_y;
  const float* cur_z = gradient_z;
  for(unsigned int k=0; k < nz; k++)
  {
    for(unsigned int j=0; j < ny; j++)
    {
      for(unsigned int i=0; i < nx; i++)
      {
        vgl_vector_3d<double> direction(*cur_x, *cur_y, *cur_z);
        det_map_entry entry;
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        entry.strength_ = direction.length();
        map_[i][j][k] = entry;

        cur_x++;
        cur_y++;
        cur_z++;
      }
    }
  }
}
det_edge_map::det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                                   double ***gradient_x, 
                                   double ***gradient_y, 
                                   double ***gradient_z)
                                   : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
  for(unsigned int k=0; k < nz; k++)
    for(unsigned int j=0; j < ny; j++)
      for(unsigned int i=0; i < nx; i++)
      {
        vgl_vector_3d<double> direction(gradient_x[i][j][k], gradient_y[i][j][k], gradient_z[i][j][k]);
        det_map_entry entry;
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        entry.strength_ = direction.length();
        map_[i][j][k] = entry;
      }
}

det_edge_map det_edge_map::nonmaxium_suppression_for_edge_detection()
{
  det_edge_nonmaximum_suppression suppress;
  det_edge_map result_cm(suppress.apply(map_));
  return result_cm;
}

det_map_entry const& det_edge_map::operator()(unsigned i, unsigned j, unsigned k) const
{
  return map_[i][j][k];
}

det_map_entry& det_edge_map::operator()(unsigned i, unsigned j, unsigned k) 
{
  return map_[i][j][k];
}

vcl_istream& operator >> ( vcl_istream& stream, det_edge_map& map)
{
  int nx, ny, nz;
  char str[80];
  
  stream >> nx >> ny >> nz;

  if(map.nx() != nx || map.ny() != ny || map.nz() != nz)
    map.resize(nx, ny, nz);

  
  for(int i = 0; i < nz; i++)
    for(int j = 0; j < ny; j++)
      for(int k = 0; k < nx; k++)
      { 
        int x, y, z;

        double dirx, diry, dirz;

        // get ride of [z=
        stream.read(str, 4);

        stream >> z ; 

        stream.read(str, 3); stream >> y ;

        stream.read(str, 3); stream >> x ;

        stream.read(str, 11); stream >> map[x][y][z].strength_;

        stream.read(str, 6); stream >> dirx >> diry >> dirz;
        map[x][y][z].dir_ = vgl_vector_3d<double>(dirx, diry, dirz); 
#if 0 
        stream.read(str, 18); stream >> offx >> offy >> offz;
        map[x][y][z].location_ = vgl_point_3d<double>(offx, offy, offz);
#endif
        // get rid of ]
        stream.read(str, 1);

      }

  return stream;
}

vcl_ostream& operator << ( vcl_ostream& stream, const det_edge_map& map)
{

  int nx = map.nx();
  int ny = map.ny();
  int nz = map.nz();
  
  stream << nx <<' ' <<  ny << ' ' << nz <<'\n';
  
  for(int k=0; k < nz; k++)
    for(int j=0; j < ny; j++)
      for(int i=0; i < nx; i++)
      {
        vgl_vector_3d<double> dir = map[i][j][k].dir_;
        vgl_point_3d<double> loc = map[i][j][k].location_;
        
        stream << "[z="<< k <<" y=" << j << " x=" << i <<"] strength=" << map[i][j][k].strength_ <<" dir=[" << dir.x() << ' ' << dir.y() << ' ' 
          << dir.z() << "] subvoxel offset=[" << loc.x() << ' ' << 
          loc.y() << ' ' << loc.z() << "]\n";
      }
  return stream;
}

