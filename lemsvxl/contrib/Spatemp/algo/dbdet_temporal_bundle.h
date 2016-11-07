//: this is contrib/spatemp/algo/dbdet_temporal_bundle
#ifndef dbdet_temporal_bundle_h
#define dbdet_temporal_bundle_h

#include <vcl_map.h>
#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <dbdet/sel/dbdet_curvelet.h>

//: class to store an individual temporal grouing 
class dbdet_temporal_bundle{

public:
    //dbdet_temporal_bundle(vcl_map<int, vcl_list<dbdet_edgel*> > bundle, dbdet_temporal_model *model);
     dbdet_temporal_bundle(const dbdet_temporal_bundle & c);

    dbdet_temporal_bundle( dbdet_temporal_model *model);
    dbdet_temporal_bundle(vcl_map<int, dbdet_curvelet*> bundle, dbdet_temporal_model *model);
    ~dbdet_temporal_bundle(){}

    //: int is the frme number and dbdet_curvelet is the member from that frame.
    vcl_map<int, dbdet_curvelet*> bundle_;
    //vcl_map<int, vcl_list<dbdet_edgel*> > bundle_;
    dbdet_temporal_model *model_;


    dbdet_curvelet * refc;
    //: to check if it has been consumed by a bigger temporal bundle
    //: need to throw out such bundles
    bool used_;

    //: saliency of the bundle
    double weight_;
    //: normalized saliency of the bundle
    double nweight_;

    //: has this bundle grouped with others
    int group_id_;

    //: is this bundle grouped one-sided
    bool one_sided_;

protected:

};
#endif
