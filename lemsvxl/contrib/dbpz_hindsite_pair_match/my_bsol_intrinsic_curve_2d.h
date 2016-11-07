#ifndef my_bsol_intrinsic_curve_2d_h_
#define my_bsol_intrinsic_curve_2d_h_



#include "bsol/bsol_intrinsic_curve_2d.h"
#include "bsol/bsol_intrinsic_curve_2d_sptr.h"


#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_vector.h>




class my_bsol_intrinsic_curve_2d : public bsol_intrinsic_curve_2d {

public:
    my_bsol_intrinsic_curve_2d(){};
    virtual ~my_bsol_intrinsic_curve_2d(){};

    //Constructor from a vcl_vector of points
    my_bsol_intrinsic_curve_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices) : bsol_intrinsic_curve_2d(new_vertices){};
    //Copy constructor
    my_bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &other) : bsol_intrinsic_curve_2d(other){};

    vcl_vector<vsol_point_2d_sptr>* getStorage(){ return storage_; };

    void changeStart( int newStart );
    void reverse();
    void coarseResample( int skipRate );

};


#endif

