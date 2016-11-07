// This is brcv/shp/dbskfg/algo/dbskfg_compute_scurve.h

#ifndef dbskfg_compute_scurve_h_
#define dbskfg_compute_scurve_h_

//:
//: \file
//: \brief  Contains code to compute shock curves from composite graph paths
//: \author Maruthi Narayanan (mn@lems.brown.edu)
//: \date   Sep 24, 2010
// 
//: \verbatim
//  Modifications
//      Maruthi Narayanan     Sep 24, 2010     Initial version
//
//: \endverbatim


#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <vcl_vector.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>
class dbsk2d_xshock_edge;

class dbskfg_compute_scurve
{
public: 
    
    // Constructor
    dbskfg_compute_scurve(vsol_box_2d_sptr bbox=0);

    // Destructor
    ~dbskfg_compute_scurve();

    // compute shock curve
    dbskr_scurve_sptr compute_curve(dbskfg_composite_node_sptr start_node,
                                    vcl_vector<dbskfg_composite_link_sptr> path,
                                    bool leaf_edge, 
                                    bool binterpolate, bool bsub_sample,
                                    double interpolate_ds, double subsample_ds,
                                    double scale_ratio=1.0,
                                    bool mirror=false);

private:

    // sample shock
    void sample_shock_link(dbskfg_composite_link_sptr link,
                           dbsk2d_xshock_edge& xshock_edge,
                           bool mirror=false);
    
    // mirror shock
    void mirror_shock(dbsk2d_xshock_edge& xshock_edge);

    // mirror angle
    double mirror_angle(double theta)
    {
        double diff(0.0);
        double output(0.0);

        if ( theta >= 0 && theta <= vnl_math::pi_over_2 )
        {
            
            diff=2*(vnl_math::pi_over_2-theta);
            output=theta+diff;
        }
        else if (theta > vnl_math::pi_over_2 && theta <= vnl_math::pi )
        {
    
            diff=2*(theta-vnl_math::pi_over_2 );
            output=theta-diff;
        }
        else if (theta > vnl_math::pi && theta <= vnl_math::pi_over_2*3 )
        {
            diff=2*(vnl_math::pi_over_2*3-theta);    
            output=theta+diff;
        }
        else
        {
            diff=2*(theta-vnl_math::pi_over_2*3);
            output=theta-diff;
        }

        return output;

    }

    // Make copy constructor private
    dbskfg_compute_scurve(const dbskfg_compute_scurve&);

    // Make assignment operator private
    dbskfg_compute_scurve& operator=(const dbskfg_compute_scurve&);

    // Have a bounding box
    vsol_box_2d_sptr bbox_;

};

#endif //dbskfg_compute_scurve_h_
