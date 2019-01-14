//: this is contrib/spatemp/algo/dbdet_contour_fragment
#ifndef dbdet_contour_fragment_h
#define dbdet_contour_fragment_h

#include <map>

#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <dbdet/sel/dbdet_curvelet.h>

//: class to store grouping of St-bundles
//: grouping is solely based on dynamics
class dbdet_contour_fragment{

public:
    
    dbdet_contour_fragment();
    ~dbdet_contour_fragment(){}

    //: int is the frame number and std::set<int> are the ids of member edges from that frame.
    std::map<int,std::set<int> > grouping_;
    std::list<dbdet_temporal_bundle *> chain_;
    //: ref hypothesis
    dbdet_temporal_bundle *tb_;


    std::vector<int> memids;

    int id_;

protected:

};
#endif
