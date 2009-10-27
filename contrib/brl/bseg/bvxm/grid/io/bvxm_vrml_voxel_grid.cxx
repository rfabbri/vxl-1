#include "bvxm_vrml_voxel_grid.h"


void bvxm_vrml_voxel_grid::write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n";
}


void bvxm_vrml_voxel_grid::write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "        color[\n";
  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i){
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j) > threshold))
          str <<"        " << (*grid_it)(i,j) << ' '<< (*grid_it)(i,j) << ' '<< (*grid_it)(i,j)
              << '\n';
      }
    }
  }
  str << "        ]\n     }\n"
      << "      coord Coordinate{\n"
      << "        point[\n";

  //write the coordinates
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j) > threshold)
         str<<"        " << i << ' ' <<j << ' ' << k <<  '\n';
      }
    }
  }
  str << "        ]\n     }\n   }\n}\n";
}


void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();

  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i){
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j) > threshold)){
          vgl_sphere_3d<float> sphere((float)i,(float)j,(float)k,0.5f);
          write_vrml_sphere(str, sphere, (*grid_it)(i,j),(*grid_it)(i,j),(*grid_it)(i,j),0);
        }
      }
    }
  }
}

void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold)
{
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();

  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i){
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j)[3]/255.0f > threshold){
          vgl_sphere_3d<float> sphere((float)i,(float)j,(float)k,0.5f);
          write_vrml_sphere(str, sphere, (*grid_it)(i,j)[0]/255.0f,(*grid_it)(i,j)[1]/255.0f,(*grid_it)(i,j)[2]/255.0f,1.0f - (*grid_it)(i,j)[3]/255.0f);
        }
      }
    }
  }
}


void bvxm_vrml_voxel_grid::write_vrml_sphere(vcl_ofstream& str, vgl_sphere_3d<float> const& sphere,
                                             const float r, const float g, const float b,
                                             const float transparency)
{
  double x0 = sphere.centre().x(), y0 = sphere.centre().y(), z0 = sphere.centre().z();
  double rad = sphere.radius();
  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' '
      << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << rad << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

void bvxm_vrml_voxel_grid::write_vrml_line_segment(vcl_ofstream& str, const vgl_line_segment_3d<int> &line, const float r, const float g, const float b,
                                                   const float transparency)
{
  str << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      emissiveColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedLineSet\n"
      <<   "{\n"
      << "      coord Coordinate{\n"
      << "       point[\n"
      << line.point1().x() << ' ' << line.point1().y() << ' ' <<line.point1().z() << '\n'
      << line.point2().x() << ' ' << line.point2().y() << ' ' <<line.point2().z() << '\n'
      << "   ]\n\n }"
      << "   coordIndex [\n"
      << "0,1   ]\n  }\n}";
}

void bvxm_vrml_voxel_grid::write_vrml_box(vcl_ofstream& str, vgl_box_3d<int> const& box,
                                          const float r , const float g , const float b ,
                                          const float transparency )
{
  int x0 = box.centroid().x(), y0 = box.centroid().y(), z0 = box.centroid().z();
  int w = box.max_point().x() - box.min_point().x();
  int h = box.max_point().y() - box.min_point().y();
  int d = box.max_point().z() - box.min_point().z();

  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Box\n"
      <<   "{\n"
      << "  size " << w << ' ' << h << ' ' << d << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

