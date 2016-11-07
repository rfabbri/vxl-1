//: this is contrib/spatemp/algo/dbdet_contour_fragment
#ifndef dbdet_contour_fragment_h
#define dbdet_contour_fragment_h

#include <vcl_map.h>

#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <dbdet/sel/dbdet_curvelet.h>

//: class to store grouping of St-bundles
//: grouping is solely based on dynamics
class dbdet_contour_fragment{

public:
    
    dbdet_contour_fragment();
    ~dbdet_contour_fragment(){}

    //: int is the frame number and vcl_set<int> are the ids of member edges from that frame.
    vcl_map<int,vcl_set<int> > grouping_;
    vcl_list<dbdet_temporal_bundle *> chain_;
    //: ref hypothesis
    dbdet_temporal_bundle *tb_;


    vcl_vector<int> memids;

    int id_;

protected:

};
#endif
