#include <Spatemp/algo/dbdet_temporal_options.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_detector_mixture.h>
#include <bsta/bsta_detector_gaussian.h>

bool compare_temporal_bundle(dbdet_temporal_bundle  b1,dbdet_temporal_bundle  b2);
bool compare_temporal_bundle_by_weight(dbdet_temporal_bundle  b1,dbdet_temporal_bundle  b2);

dbdet_temporal_options::dbdet_temporal_options(dbdet_curvelet * refc)
{
    refc_=refc;
}
dbdet_temporal_options::~dbdet_temporal_options()
{

}
void
dbdet_temporal_options::merge_the_models()
{
    double epsilon_error=0.5;
    vcl_map<int, dbdet_curvelet*>::iterator cvletiter;
    typedef vcl_map<int, vcl_list<dbdet_temporal_bundle> >::iterator frame_iter;

    list_options_.sort(compare_temporal_bundle);
    //:  merge the models of pair of edges which have close enough parameters.
    for(frame_iter fupper=options_.begin();fupper!=options_.end();fupper++)
    {
        for(biter biterator=fupper->second.begin();biterator!=fupper->second.end();biterator++)
        {
            frame_iter flower=fupper;
            if(++flower!=options_.end())
            {
                for(;flower!=options_.end();flower++)
                {
                    for(biter sub_biterator=flower->second.begin();sub_biterator!=flower->second.end();sub_biterator++)
                    {
                       
                        if(check_compatibility((*biterator), (*sub_biterator)))
                        {
                            //if(biterator->model_->is_model_intersect(sub_biterator->model_,epsilon_error) && 
                            //    sub_biterator->model_->is_model_intersect(biterator->model_,epsilon_error))
                            {
                                
                                //: we will treat biterator as the anchor 
                                vcl_map<int, dbdet_curvelet*> tempmembers;
                                for(cvletiter=biterator->bundle_.begin();cvletiter!=biterator->bundle_.end();cvletiter++)
                                    tempmembers[cvletiter->first]=cvletiter->second;
                                for(cvletiter=sub_biterator->bundle_.begin();cvletiter!=sub_biterator->bundle_.end();cvletiter++)
                                    tempmembers[cvletiter->first]=cvletiter->second;

                                if(dbdet_temporal_curvature_velocity_model * m1=dynamic_cast<dbdet_temporal_curvature_velocity_model*>( biterator->model_))
                                {
                                    if(dbdet_temporal_curvature_velocity_model * m2=dynamic_cast<dbdet_temporal_curvature_velocity_model*> (sub_biterator->model_))
                                    {
                                        dbdet_temporal_curvature_velocity_model * tempmodel=intersect(*m1,*m2);
                                        for(cvletiter=sub_biterator->bundle_.begin();cvletiter!=sub_biterator->bundle_.end();cvletiter++)
                                            biterator->bundle_[cvletiter->first]=cvletiter->second;
                                        tempmodel->set_num_obs(tempmembers.size());
                                        sub_biterator->used_=true;
                                        biterator->model_=tempmodel;
                                    }
                                }
                                else if(dbdet_temporal_normal_velocity_model * m1=dynamic_cast<dbdet_temporal_normal_velocity_model*>( biterator->model_))
                                {
                                    if(dbdet_temporal_normal_velocity_model * m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (sub_biterator->model_))
                                    {
                                        //: make a new bundle
                                        dbdet_temporal_normal_velocity_model  * tempmodel=intersect(m1,m2);
                                        tempmodel->ref_curvelet=biterator->refc;
                                        tempmodel->compute_betas();
                                        if(tempmodel!=NULL)
                                        {
                                            tempmodel->set_num_obs(tempmembers.size());
                                            dbdet_temporal_bundle newbundle(tempmembers,tempmodel);
                                            newbundle.refc=biterator->refc;
                                            second_list_options_.push_back(newbundle);
                                            fupper->second.push_back(newbundle);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //for(biter biterator=list_options_.begin();biterator!=list_options_.end();)
    //{
    //    if(biterator->used_)
    //    {
    //        biterator=list_options_.erase(biterator);

    //    }
    //    else
    //        biterator++;
    //}
}


//: bundles are comaptible if they  have different member frames.
bool 
dbdet_temporal_options::check_compatibility(dbdet_temporal_bundle & b1, dbdet_temporal_bundle & b2)
{
    vcl_map<int,dbdet_curvelet* >::iterator iter1;
    vcl_map<int,dbdet_curvelet* >::iterator iter2;
    //if(b1.bundle_.size()+b2.bundle_.size()>2)
    //    return false;
    bool flag=true;
    for(iter1=b1.bundle_.begin();iter1!=b1.bundle_.end();iter1++)
    {
        
        for(iter2=b2.bundle_.begin();iter2!=b2.bundle_.end();iter2++)
        {
            if(iter1->first!=0 && iter2->first!=0)
            {
                if(iter1->first==iter2->first)
                    flag=false;
            }
        }
    }
    return flag;
}
void
dbdet_temporal_options::add_temporal_bundle(dbdet_temporal_bundle & b)
{
    list_options_.push_back(b);
}

void
dbdet_temporal_options::refine_bundles(int t)
{
    typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
    typedef bsta_mixture<gauss_type> bsta_mix_t;
    typedef bsta_num_obs<bsta_mix_t> mix_gauss_type;
    float init_var=0.3;
    bsta_gauss_f1 init_gauss(0.0f,init_var);
    mix_gauss_type mixture;
    bsta_mg_statistical_updater<bsta_mix_t> updater(init_gauss, 10);


    vcl_list<dbdet_temporal_bundle>::iterator iter=list_options_.begin();
    for(;iter!=list_options_.end();iter++)
    {
        if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*> (iter->model_))
            updater(mixture,m->b0);

    }

  typedef bsta_g_mdist_detector<bsta_gauss_f1> detector_type;
  typedef bsta_top_weight_detector<mix_gauss_type,detector_type> weight_detector_type;

  bsta_g_mdist_detector<bsta_gauss_f1> det(2.5);
  bsta_top_weight_detector<mix_gauss_type,detector_type> detector_top(det, 0.1);

  //: only retaining the beta values 
  for(unsigned i=0;i<mixture.num_components();i++)
  {
      dbdet_temporal_normal_velocity_model * model=new dbdet_temporal_normal_velocity_model(mixture.distribution(i).mean(),t);
      model->ref_curvelet=refc_;
      vcl_map<int,dbdet_curvelet*> dummy;
      dummy[t]=0;
      dbdet_temporal_bundle b(dummy,model);
      b.refc=refc_;
      second_list_options_.push_back(b);
      options_[t].push_back(b);
  }
  //for(;iter!=list_options_.end();iter++)
  //{
  //    if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*> (iter->model_))
  //    {
  //        for(unsigned i=0;i<mixture.num_components();i++)
  //        {
  //            bool result=false;
  //            detector_top.detect(mixture.distribution(i),m->b0,result)
  //                if(result)
  //                {

  //                }
  //        }
  //    }
  //}
        
    

}

bool compare_temporal_bundle(dbdet_temporal_bundle  b1,dbdet_temporal_bundle  b2)
{
    if (vcl_abs(b1.bundle_.begin()->first)<vcl_abs(b2.bundle_.begin()->first))
        return true;
    else if (vcl_abs(b1.bundle_.begin()->first)==vcl_abs(b2.bundle_.begin()->first))
    {
        if(b1.bundle_.begin()->first<b2.bundle_.begin()->first)
            return false;
        else if(b1.bundle_.begin()->first==b2.bundle_.begin()->first)
            return false;
        else
            return true;
    }
    else
        return false;
}
bool compare_temporal_bundle_by_weight(dbdet_temporal_bundle  b1,dbdet_temporal_bundle  b2)
{
    if (b1.nweight_>b2.nweight_)
        return true;
    return false;
}

void dbdet_temporal_options::sort_by_weight()
{
    list_options_.sort(compare_temporal_bundle_by_weight);
}
void dbdet_temporal_options::normalize_weights()
{
    double tot_weight_=0;
    vcl_list<dbdet_temporal_bundle>::iterator list_iter;
    for(list_iter=list_options_.begin();list_iter!=list_options_.end();list_iter++)
    {   
        if(!list_iter->used_ && list_iter->bundle_.size()>1) 
        {
            tot_weight_+=list_iter->weight_;
        }
    }
    if(tot_weight_>0)
    {
        for(list_iter=list_options_.begin();list_iter!=list_options_.end();list_iter++)
        {   
            if(!list_iter->used_ && list_iter->bundle_.size()>1) 
            {
                list_iter->nweight_=list_iter->weight_/tot_weight_;
                list_iter->weight_=list_iter->nweight_;
            }

        }
    }
       
}

void dbdet_temporal_options::print()
{
    vcl_list<dbdet_temporal_bundle>::iterator list_iter;
    for(list_iter=list_options_.begin();list_iter!=list_options_.end();list_iter++)
    {   
        if(!list_iter->used_ && list_iter->bundle_.size()>1) 
        {
            list_iter->model_->print_model();
            refc_->curve_model->print_info();
            vcl_cout<<"  weight"<<list_iter->nweight_;
            vcl_cout<<" # "<<list_iter->bundle_.size()<<"\n";
        }
    }
}

