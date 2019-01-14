#ifndef texture_map_generator_h
#define texture_map_generator_h

#include "poly_cam_observer.h"
#include "obj_observable.h"

#include <vil/vil_image_view.h>
#include <vsol/vsol_point_2d.h>

#include <list>
#include <vector>
#include <string>

#include <bgeo/bgeo_lvcs.h>

class texture_map_generator
{
public:
  //: constructors
  texture_map_generator(std::vector<poly_cam_observer*> observers) {observers_ = observers;}

  //: generate the texture map
  bool texture_map_generator::generate_texture_map(obj_observable* obj, std::string texture_filename, bgeo_lvcs lvcs);


private:
vgl_vector_3d<double> compute_face_normal_lvcs(dbmsh3d_face* face, bgeo_lvcs lvcs);

std::vector<poly_cam_observer*> observers_;

};


#endif

