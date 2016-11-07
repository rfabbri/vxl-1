#ifndef dbdet_spherical_histogram_h_
#define dbdet_spherical_histogram_h_

#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>

class dbdet_spherical_histogram: public vbl_ref_count 
{

public:
    dbdet_spherical_histogram();
    ~dbdet_spherical_histogram(){}

    bool update(float theta,float phi);
    void print();

   dbdet_spherical_histogram & operator+=(dbdet_spherical_histogram & c);

   float get_theta(int i);
   float get_phi(int i,int j);
    vcl_map<int,vcl_vector<float> > hist_;
    int theta_partitions_; //: 8 
    vcl_map<int,int> phi_partitions_; 

protected:


float theta_min_;
float theta_max_;

float phi_min_;
float phi_max_;

};

#endif
