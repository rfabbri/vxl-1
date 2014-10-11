//---------------------------------------------------------------------
// This is contrib/maruthi/dbskfg/alog/dbskfg_app_curve_match.h
//:
// \file
// \brief This is a curve matching class subclassed from dbcvr_cvmatch_even
//        to match appearance for scurves
//        Dynamic programming core is the same with dbcvr curve matching
//        interval cost computation is different and specific to shock curves.
//
// \author
//  Maruthi Narayanan - October 9th , 2014
//
//-------------------------------------------------------------------------
#ifndef _dbskfg_app_curve_match_h
#define _dbskfg_app_curve_match_h

#include <dbcvr/dbcvr_cvmatch_even.h>
#include <vnl/vnl_matrix.h>
#include <vl/sift.h>
#include <vnl/vnl_vector.h>

extern "C" {
#include <vl/mathop.h>
}

class dbskfg_app_curve_match : public dbcvr_cvmatch_even 
{
public:
    
    dbskfg_app_curve_match(vnl_matrix<vl_sift_pix> sift_c1,
                           vnl_matrix<vl_sift_pix> sift_c2);
    
    virtual ~dbskfg_app_curve_match();
    
    //overwritten DP functions
    virtual void initializeDPCosts();
    virtual double computeIntervalCost (int i, int ip, int j, int jp);
    
    
protected:
    
    //shock curves to match
    vnl_matrix<vl_sift_pix> sift_c1_;
    vnl_matrix<vl_sift_pix> sift_c2_;

 
    double descr_cost(vnl_vector<vl_sift_pix>& v1,
                      vnl_vector<vl_sift_pix>& v2)
    {
        VlFloatVectorComparisonFunction Chi2_distance =    
            vl_get_vector_comparison_function_f (VlDistanceChi2) ;


        vl_sift_pix result_final[1];

        vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                                 v1.size(),
                                                 v1.data_block(),
                                                 1,
                                                 v2.data_block(),
                                                 1,
                                                 Chi2_distance);
        
        return 0.5*result_final[0];
    }
};


#endif // _dbskfg_app_curve_match_h
