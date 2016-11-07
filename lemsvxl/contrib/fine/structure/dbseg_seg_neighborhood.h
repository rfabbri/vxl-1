//This is contrib/fine/gui/dbseg_seg_neighborhood.h
#ifndef dbseg_seg_neighborhood_h_
#define dbseg_seg_neighborhood_h_

//:
// \file
// \the data structure for the neighborhood adjacancy map for segmentation
// \author Eli Fine
// \date 7/29/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <list>

using namespace std;
class dbseg_seg_neighborhood {

public:
    dbseg_seg_neighborhood(int numSegs);

    //for loading
    dbseg_seg_neighborhood(vector<list<int>> listL);

    ~dbseg_seg_neighborhood();
    //returns false if n already was a neighbor of home
    void add_neighbor(int n, int home);
    //returns false if n was not a neighbor of home
    void remove_neighbor(int n, int home);
    //returns the pointer to the list of neighbors of object ID o
    list<int>& get_neighbors(int o);
    //returns the vector of the list of all neighbors
    vector<list<int>>& get_neighbor_list();
    void clear_neighbors(int o);
    //creates a blank list in this object's spot because it has no neighbors
    void add_no_neighbors(int o); 

    bool is_neighbors(int o1, int o2);

    void create_space();
    //
    
    
protected:
    //a vector of the id numbers of objects with lists of the ids of its neighbors
    vector<list<int>> neighbor_list;




};

#endif

