//: this is contrib/spatemp/algo/dbdet_temporal_options
#ifndef dbdet_temporal_options_h
#define dbdet_temporal_options_h

#include <vcl_map.h>
#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>

//: class to store the temporal hypotheses for each edge
//: temporal bundles should be able to distinguish between time 
//: stamps of each bundle.
typedef vcl_list<dbdet_temporal_bundle>::iterator biter;
class dbdet_temporal_options: public vbl_ref_count
{

public:
    dbdet_temporal_options(dbdet_curvelet * refc);
    ~dbdet_temporal_options();

    //: add a temporal bundle to the list
    void add_temporal_bundle(dbdet_temporal_bundle & b);
    //: function creates basic unit of model over all frames.
    void form_all_possible_models();

    //: function to merge smaller options 
    void merge_the_models();

    void refine_bundles(int t);
   
    //: check if they have different memberships
    bool check_compatibility(dbdet_temporal_bundle & b1, dbdet_temporal_bundle & b2);
//: to normalize the weights for each 
    void normalize_weights();

    void sort_by_weight();
//: to print the options
    void print();
    //:
    dbdet_curvelet * refc_;
    vcl_list<dbdet_temporal_bundle> list_options_;
    vcl_list<dbdet_temporal_bundle> second_list_options_;

    vcl_map<int, vcl_list<dbdet_temporal_bundle> > options_;

    vcl_list<dbdet_temporal_bundle> st_bundles_;

protected:
    //: function to check it two options can be merged
//    bool check_compatibility(dbdet_temporal_bundle & b1, dbdet_temporal_bundle & b2);


    
};


#endif
