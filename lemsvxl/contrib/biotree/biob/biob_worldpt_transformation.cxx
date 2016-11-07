#include <bio_defs.h>
#include <biob/biob_worldpt_transformation.h>

#include <vcl_cassert.h>

worldpt biob_worldpt_transformation::operator()(worldpt pt){
  return convert(matrix_(convert(pt)));
}

biob_worldpt_transformation::biob_worldpt_transformation(vgl_h_matrix_3d<double> matrix)
  : matrix_(matrix){}
  
//: just so an array of these can be constructed---not ready to use
biob_worldpt_transformation::biob_worldpt_transformation() {}

  
vgl_homg_point_3d<double> biob_worldpt_transformation::convert(worldpt pt){
  return vgl_homg_point_3d<double>(pt.x(),pt.y(),pt.z());
}

worldpt biob_worldpt_transformation::convert(vgl_homg_point_3d<double> pt){
double x,y,z;
  bool successflag = pt.get_nonhomogeneous(x,y,z);
  assert(successflag);
  return worldpt(x,y,z);
}

worldpt biob_worldpt_transformation::preimage(worldpt pt){
  return convert(matrix_.preimage(convert(pt)));
}

biob_worldpt_transformation biob_worldpt_transformation::inverse() const{
    return biob_worldpt_transformation(matrix_.get_inverse());
}
