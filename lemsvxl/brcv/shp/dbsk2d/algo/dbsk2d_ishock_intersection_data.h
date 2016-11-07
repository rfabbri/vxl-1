// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_intersection_data.h
#ifndef dbsk2d_ishock_intersection_data_h_
#define dbsk2d_ishock_intersection_data_h_
//:
// \file
// \brief Stores intersection data of two intrinsic shock curves
// \author Amir Tamrakar
// \date 02/02/05
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 07/07/2005    Moved to dbsk2d/algo
//
// \endverbatim

//: Stores intersection data of two intrinsic shock curves
// This class is used for storing the results of intrinsic intersection
//computation between two shock paths
class dbsk2d_ishock_intersection_data
{
public:
  double   R;       ///< radius or time at intersection
  double   LSLtau;  ///< Left shock Left tau
  double   LSRtau;  ///< Left shock right tau
  double   RSLtau;  ///< Right shock Left tau
  double   RSRtau;  ///< Right shock Right tau

  double   angle;   ///< just for point-line contacts at a junction

  dbsk2d_ishock_intersection_data()
  {
    R=ISHOCK_DIST_HUGE; 
    LSLtau=0; 
    LSRtau=0; 
    RSLtau=0; 
    RSRtau=0;
    
    angle=ISHOCK_DIST_HUGE;
  }

  ~dbsk2d_ishock_intersection_data() {}
};

#endif //dbsk2d_ishock_intersection_data_h_
