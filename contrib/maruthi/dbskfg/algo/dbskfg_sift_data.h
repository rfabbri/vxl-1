// This is a class to represent 
#ifndef dbskfg_sift_data_h_
#define dbskfg_sift_data_h_

//:
// \file
// \brief A class to hold the sift parameters
//  
// \author Maruthi Narayanan
// \date  12/10/13
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_2d.h>

class dbskfg_sift_data
{

public:

    // Constructor
    dbskfg_sift_data(){};

    // Destructor
    ~dbskfg_sift_data(){};

    vgl_point_2d<double> location_;

    double radius_;

    double phi_;

};

#endif // dbskfg_sift_data_h_


