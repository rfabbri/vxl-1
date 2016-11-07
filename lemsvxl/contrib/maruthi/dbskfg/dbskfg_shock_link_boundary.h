// This is a class to represent 
#ifndef dbskfg_shock_link_boundary_h_
#define dbskfg_shock_link_boundary_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a shock graph boundary
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgl/vgl_point_2d.h>
#include <vcl_vector.h>

class dbskfg_shock_link_boundary
{

public:

    // Constructor
dbskfg_shock_link_boundary():original_contour_id_(0),point_(0,0){}

    // Destructor
    ~dbskfg_shock_link_boundary(){contour_.clear();contour_ids_.clear();}

    //Attributes
    
    // Original contour id this point or contour came from
    unsigned int original_contour_id_;

    // Boundary could be a contour
    vcl_vector<vgl_point_2d<double> > contour_;

    // Boundary could be a point
    vgl_point_2d<double> point_;

    // Holds all the node ids that represent the points of the vsol
    vcl_vector<unsigned int> contour_ids_;

    bool operator<( const dbskfg_shock_link_boundary& rhs ) const
    {
      
        if ( rhs.contour_ids_.back() == this->contour_ids_.front())
        {
            return true;
        }

        if ( rhs.contour_ids_.front() == this->contour_ids_.back())
        {
            return false;
        }
    }

    bool operator==(const dbskfg_shock_link_boundary& rhs ) const
    {

        if ( (rhs.contour_ids_.front() == this->contour_ids_.front()) && 
             (rhs.contour_ids_.back() == this->contour_ids_.back())
            )
        {
            return true;
        }

        if ( (rhs.contour_ids_.back() == this->contour_ids_.front()) && 
             (rhs.contour_ids_.front() == this->contour_ids_.back())
            )
        {
            return true;
        }


    }

    void print()
    {
        if ( contour_.size() )
        {
            vcl_cout<<"Contour start:  "<<contour_.front()<<" stop: "
                    <<contour_.back()<<vcl_endl;
        }

        vcl_cout<<"Point: "<<point_<<vcl_endl;
        vcl_cout<<vcl_endl;

    }
};

#endif // dbskfg_shock_link_boundary_h_


