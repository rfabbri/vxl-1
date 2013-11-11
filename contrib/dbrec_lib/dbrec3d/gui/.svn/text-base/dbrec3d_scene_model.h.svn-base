// This is dbrec3d_scene_model.h
#ifndef dbrec3d_scene_model_h
#define dbrec3d_scene_model_h

//:
// \file
// \brief A class data models used to visualize a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  26-Oct-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QImage>

#include <boxm/boxm_scene_base.h>
#include <bsta/bsta_histogram.h>
#include <vnl/vnl_float_3.h>

enum dbrec3d_scene_type {
  GRADIENTS = 0,
  UNKNOWN
};

//: A class to hold the model of a boxm scene which is visualized in other widgets
class dbrec3d_scene_model
{
public:
  //: Constructor - from a boxm_scene_base
  dbrec3d_scene_model(): scene_base_(NULL){}
  dbrec3d_scene_model(boxm_scene_base_sptr scene);
  
  //: Return a histogram image of the data in this scene
  QImage histogram_image();
  
  //: min, max values of data that is being visualized
  float min_val() { return hist_.min(); }
  float max_val() { return hist_.max(); }
  int interval() { return hist_.nbins(); }
  
  boxm_scene_base_sptr scene() {return scene_base_; }
  
  
  const boxm_scene_base_sptr scene_base_;
  
  //: Determines the type of the scene, and how it is rendered
  //const dbrec3d_scene_type scene_type_;
  
  bsta_histogram<float> hist_;
  
};

//: A class to hold a simple arrow
class dbrec3d_arrow
{
public:
  dbrec3d_arrow( vnl_float_3 pos, vnl_float_3 end_pos, float length): pos_(pos), end_pos_(end_pos), length_(length){}
  
  void draw();
  
  // Member variables
  vnl_float_3 pos_;
  vnl_float_3 end_pos_; 
  float length_;
};

#endif
