// This is dbdet_spatial_temporal_options.h
#ifndef dbdet_spatial_temporal_options_h
#define dbdet_spatial_temporal_options_h
//:
//\file
//\brief Spatial Temporal bundles
//\author Vishal Jain
//\date 04/10/08
//
//\endverbatim

#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>

#include <dbdet/sel/dbdet_curvelet_map.h>
#include <Spatemp/algo/dbdet_temporal_options_sptr.h>


//: This class  is responsible for storing information per edgel 
class dbdet_spatial_temporal_options: public vbl_ref_count 
{
public:
    dbdet_spatial_temporal_options(){st_options_.clear();}
    ~dbdet_spatial_temporal_options(){}

    //: options map indexed by curvelets
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >  st_options_;
    
    void normalize_weights();
    bool compute_max_k();
    float max_k(){return max_k_;}

private:
    float max_k_;

};

#endif // dbdet_spatial_temporal_options_h
