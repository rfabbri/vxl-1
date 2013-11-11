//this is contrib/bm/dncn/dncn_neighborhood_2d.h
#ifndef dncn_neighborhood_2d_h_
#define dncn_neighborhood_2d_h_

#include<vbl/vbl_ref_count.h>

#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

class dncn_neighborhood_2d: public vbl_ref_count
{
public:
    typedef vcl_vector<vgl_point_2d<unsigned> > neighborhood_type;

    dncn_neighborhood_2d(){}

    dncn_neighborhood_2d(neighborhood_type& neighborhood): neighborhood_(neighborhood){}

    ~dncn_neighborhood_2d(){}

    //setters
    void set_neighborhood( neighborhood_type& nbrhd ){ neighborhood_ = nbrhd; }

    //getters
    neighborhood_type neighborhood(){ return neighborhood_; }

    neighborhood_type::iterator neighborhood_begin(){ return this->neighborhood_.begin(); }

    neighborhood_type::iterator neighborhood_end(){ return this->neighborhood_.end(); }

    unsigned num_neighbors(){ return neighborhood_.size(); }

protected:
    neighborhood_type neighborhood_;
};
#endif //dncn_neighborhood_2d_