#include "det_cylinder_map.h"
#include <vcl_fstream.h>

det_cylinder_map::det_cylinder_map(unsigned nx, unsigned ny, unsigned nz)
  : map_(nx, ny, nz), nx_(nx), ny_(ny), nz_(nz)
{
}

det_cylinder_map::det_cylinder_map(vbl_array_3d<det_map_entry> const & map)
  : map_(map)
{
  nx_ = map.get_row1_count();
  ny_ = map.get_row2_count();
  nz_ = map.get_row3_count();
}

void det_cylinder_map::resize(int nx, int ny, int nz)
{
  map_.resize(nx, ny, nz);

  nx_ = nx;
  ny_ = ny;
  nz_ = nz;
}

det_map_entry const& det_cylinder_map::operator()(unsigned i, unsigned j, unsigned k) const
{
  return map_[i][j][k];
}

det_map_entry& det_cylinder_map::operator()(unsigned i, unsigned j, unsigned k) 
{
  return map_[i][j][k];
}

vcl_istream& operator >> ( vcl_istream& stream, det_cylinder_map& map)
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

vcl_ostream& operator << ( vcl_ostream& stream, const det_cylinder_map& map)
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
        if (dir != vgl_vector_3d<double> (0, 0, 0)) {
        stream << "[z="<< k <<" y=" << j << " x=" << i <<"] strength=" << map[i][j][k].strength_ <<" dir=[" << dir.x() << ' ' << dir.y() << ' ' 
          << dir.z() << "] subvoxel offset=[" << loc.x() << ' ' << 
          loc.y() << ' ' << loc.z() << "]\n";
        }
      }
  return stream;
}

