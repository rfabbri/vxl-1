#include "dbdet_temporal_bundle.h"

dbdet_temporal_bundle::dbdet_temporal_bundle(const dbdet_temporal_bundle & c)
{
    bundle_=c.bundle_;
    model_=c.model_;
    refc=c.refc;
    used_=false;
    group_id_=-1;
    weight_=1;
    nweight_=0;
    one_sided_=false;

}
dbdet_temporal_bundle::dbdet_temporal_bundle(vcl_map<int, dbdet_curvelet*>  bundle, dbdet_temporal_model *model):model_(model)
{
    bundle_=bundle;
    //if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*> (model))
    //    model_=new dbdet_temporal_normal_velocity_model(*m);
    used_=false;
    group_id_=-1;
    weight_=1;
    nweight_=0;
    one_sided_=false;
}
dbdet_temporal_bundle::dbdet_temporal_bundle( dbdet_temporal_model *model)

{
    bundle_.clear();
    model_=model;
    used_=false;
    group_id_=-1;
    weight_=1;
    nweight_=0;
    one_sided_=false;
}
