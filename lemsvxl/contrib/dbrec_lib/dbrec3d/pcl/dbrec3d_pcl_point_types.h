#ifndef dbrec3d_pcl_point_types_h
#define dbrec3d_pcl_point_types_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11/11/11
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <pcl/point_types.h>

namespace dbrec3d_pcl_point_types {
  
  //: A pcl point to store an id and a given 
  struct EIGEN_ALIGN16 PointClassId
  {
    PCL_ADD_POINT4D; // This adds the members x,y,z which can also be accessed using the point (which is float[4])
    int class_id;
    float weight;  //could represent response, distance so on
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };
  
  
  inline std::ostream& operator << (std::ostream& os, const PointClassId& p)
  {
    os << "(" << p.x<<","<<p.y<<","<<p.z<<") ID: "<<p.class_id <<", Weight: "<<p.weight;
    return (os);
  }
}//end of dbrec3d_pcl_point_types namespace


POINT_CLOUD_REGISTER_POINT_STRUCT (dbrec3d_pcl_point_types::PointClassId,           //XYZ + "id" (as fields)
                                   (float, x, x)
                                   (float, y, y)
                                   (float, z, z)
                                   (int, class_id, class_id)
                                   (float, weight, weight)
                                   )

#endif
