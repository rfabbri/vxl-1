#include "dbdet_spatial_temporal_options.h"
#include "dbdet_temporal_options.h"
#include "dbdet_temporal_bundle.h"
void
dbdet_spatial_temporal_options::normalize_weights()
{
    double tot_weight_=0;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_list<dbdet_temporal_bundle>::iterator list_iter;

    for(cv_iter=st_options_.begin();cv_iter!=st_options_.end();cv_iter++)
    {
        for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
        {   
            if(!list_iter->used_ && list_iter->bundle_.size()>1) 
            {
                tot_weight_+=list_iter->weight_;
            }
        }
    }
    if(tot_weight_>0)
    {
        for(cv_iter=st_options_.begin();cv_iter!=st_options_.end();cv_iter++)
        {
            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
            {   
                if(!list_iter->used_ && list_iter->bundle_.size()>1) 
                {
                    list_iter->nweight_=list_iter->weight_/tot_weight_;
                    list_iter->weight_=list_iter->nweight_;
                }
            }
        }
    }
}

bool dbdet_spatial_temporal_options::compute_max_k()
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter=st_options_.begin();
    max_k_=0.0;
    for(;iter!=st_options_.end();iter++)
    {
        if(dbdet_CC_curve_model_3d * ccmodel=dynamic_cast<dbdet_CC_curve_model_3d *> (iter->first->curve_model))
        {    
            if(vcl_fabs(ccmodel->k)>max_k_)
                max_k_=vcl_fabs(ccmodel->k);
        }
    }
    return true;
}
