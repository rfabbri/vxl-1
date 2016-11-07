// This is dbdet_spatial_temporal_derivatives.h
#ifndef dbdet_spatial_temporal_derivatives_h
#define dbdet_spatial_temporal_derivatives_h
//:
//\file
//\brief Spatial Temporal bundles
//\author Vishal Jain
//\date 11/03/08
//
//\endverbatim

#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>

#include <dbdet/sel/dbdet_curvelet_map.h>
#include <Spatemp/algo/dbdet_temporal_model.h>


//: This class  is responsible for storing derivatives information per edgel 
class dbdet_spatial_temporal_derivatives//: public vbl_ref_count 
{
public:
    dbdet_spatial_temporal_derivatives(){}
    ~dbdet_spatial_temporal_derivatives(){}
  
    float tx;
    float ty;
    float x;
    float y;
    //: vector of beta
    vcl_vector<float> b;
    //: for each beta there might be few bt's
    vcl_map<float,vcl_vector<float> > bt;
    //: for each beta and curvature ther emight be few bs
    vcl_map<float,vcl_map<float, vcl_vector<float> > > bs;
    //: stores the curvelets on this edge
    vcl_vector<dbdet_curvelet*> refcs_;
    
    vcl_vector<dbdet_second_order_velocity_model> models_;

    //: function to compute the betas from forward beta map
    bool compute_b(vcl_vector<float> & forwardmap);

    //: function to compute temporal derivatives of beta
    bool compute_bt(vcl_vector<float>& backmap);

    //: function to compute spatial derivatives of beta
    bool compute_bs(vcl_map<dbdet_curvelet*,vcl_vector<float> > & k_bs,vcl_map<dbdet_curvelet*,float > & k_bs_ds, vcl_map<dbdet_curvelet*,dbdet_edgel* > & k_bs_edges);

    bool compute_models();

private:
    
};

#endif // dbdet_spatial_temporal_derivatives_h
