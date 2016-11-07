#include "dbdet_spatial_temporal_derivatives.h"
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_detector_mixture.h>
#include <bsta/bsta_detector_gaussian.h>

bool 
dbdet_spatial_temporal_derivatives::compute_b(vcl_vector<float>  & forwardmap)
{
    b.clear();
    typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
    typedef bsta_mixture<gauss_type> bsta_mix_t;
    typedef bsta_num_obs<bsta_mix_t> mix_gauss_type;
    float init_var=0.1;

    bsta_gauss_f1 init_gauss(0.0f,init_var);
    mix_gauss_type mixture;
    bsta_mg_statistical_updater<bsta_mix_t> updater(init_gauss, 10);

    
    for (unsigned i=0;i<forwardmap.size();i++)
      updater(mixture,forwardmap[i]);


  //: only retaining the beta values 
  for(unsigned i=0;i<mixture.num_components();i++)
      b.push_back(mixture.distribution(i).mean());
    return true;
}

bool 
dbdet_spatial_temporal_derivatives::compute_bt(vcl_vector<float>  & backmap)
{
    bt.clear();
    //: for each value of beta
    for (unsigned i=0;i<b.size();i++)
    {
        //: compute beta_t
        vcl_vector<float> bts;
        for (unsigned j=0;j<backmap.size();j++)
        {
            float beta_t=(b[i]-backmap[j]);
            if(b[i]*backmap[j]>0)
               bts.push_back(beta_t);                
            
        }
        if(bts.size()>0)
            bt[b[i]]=bts;
    }
    return true;
}
bool 
dbdet_spatial_temporal_derivatives::compute_bs(vcl_map<dbdet_curvelet*,vcl_vector<float> > & k_bs,vcl_map<dbdet_curvelet*,float > & k_bs_ds, vcl_map<dbdet_curvelet*,dbdet_edgel* > & k_bs_edges)
{
    bs.clear();
    float c_nx=-ty;
    float c_ny=tx;
    //: for each value of beta
    for (unsigned i=0;i<b.size();i++)
    {
        vcl_map<dbdet_curvelet*,vcl_vector<float> >::iterator iter_curvelets;
        vcl_map<dbdet_curvelet*,float >::iterator iter_curvelets_ds;
        vcl_map<dbdet_curvelet*,dbdet_edgel* >::iterator  k_bs_edges_iter;
        vcl_map<float, vcl_vector<float> > k_bs_;

        
        for (iter_curvelets=k_bs.begin(),iter_curvelets_ds=k_bs_ds.begin(),k_bs_edges_iter=k_bs_edges.begin();iter_curvelets!=k_bs.end();iter_curvelets++,iter_curvelets_ds++,k_bs_edges_iter++)
        {
            float nx=-vcl_sin(k_bs_edges_iter->second->tangent);
            float ny=vcl_cos(k_bs_edges_iter->second->tangent);

            if(dbdet_CC_curve_model_3d * ccmodel=dynamic_cast<dbdet_CC_curve_model_3d *> (iter_curvelets->first->curve_model))
            {
                float k=ccmodel->k;
                for(unsigned l=0;l<iter_curvelets->second.size();l++)
                {
                    float bl=iter_curvelets->second[l];
                    //condition to avoid taking difference of b's with different sign
                    if(bl*nx*b[i]*c_nx+bl*ny*b[i]*c_ny>0)
                        k_bs_[k].push_back((iter_curvelets->second[l]-b[i])/iter_curvelets_ds->second);
                }
            }

        }
        bs[b[i]]= k_bs_;
    }
    return true;
}


bool 
dbdet_spatial_temporal_derivatives::compute_models()
{
    models_.clear();
    for(unsigned i=0;i<b.size();i++)
    {
        float b0=b[i];
        for(unsigned j=0;j<bt[b0].size();j++)
        {
            float bt0=bt[b0][j];
            vcl_map<float,vcl_vector<float> >::iterator iter;
            for(iter=bs[b0].begin();iter!=bs[b0].end();iter++)
            { 
                float k0=iter->first;
                for(unsigned k=0;k<iter->second.size();k++)
                {
                    float bs0=iter->second[k];
                    models_.push_back(dbdet_second_order_velocity_model(b0,k0,bt0,bs0));
                }
            }   
        }
    }
    return true;
}
