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

#include <map>
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
    std::vector<float> b;
    //: for each beta there might be few bt's
    std::map<float,std::vector<float> > bt;
    //: for each beta and curvature ther emight be few bs
    std::map<float,std::map<float, std::vector<float> > > bs;
    //: stores the curvelets on this edge
    std::vector<dbdet_curvelet*> refcs_;
    
    std::vector<dbdet_second_order_velocity_model> models_;

    //: function to compute the betas from forward beta map
    bool compute_b(std::vector<float> & forwardmap);

    //: function to compute temporal derivatives of beta
    bool compute_bt(std::vector<float>& backmap);

    //: function to compute spatial derivatives of beta
    bool compute_bs(std::map<dbdet_curvelet*,std::vector<float> > & k_bs,std::map<dbdet_curvelet*,float > & k_bs_ds, std::map<dbdet_curvelet*,dbdet_edgel* > & k_bs_edges);

    bool compute_models();

private:
    
};

#endif // dbdet_spatial_temporal_derivatives_h
