//This is contrib/fine/gui/dbseg_seg_tree.h
#ifndef dbseg_seg_tree_h_
#define dbseg_seg_tree_h_

//:
// \file
// \the representation of the hierarchical tree for segmentation
// \author Eli Fine
// \date 7/29/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <list>

#include "dbseg_seg_spatial_object.h"

//class seg_object;
using namespace std;

template <class T>
class dbseg_seg_tree {

public:
    dbseg_seg_tree(int numSegs);

    //for loading
    dbseg_seg_tree(list<int> validL, vector<dbseg_seg_spatial_object<T>*> objL, vector<int> parentL, vector<list<int>> childL, vector<list<int>> depthL);

    ~dbseg_seg_tree();
    //returns false if c was already a child of p
    bool add_child(int c, int p);
    //returns false if c was not a child of p
    bool remove_child(int c, int p);
    //returns the number of children of an object
    const int get_child_count(int o);
    //returns true if object o has a parent
    bool has_parent(int o);
    //returns list of children
    list<int>& get_children(int o);
    //sets the parent of c to p
    void set_parent(int p, int c);
    //returns parent
    const int get_parent(int o);
    //returns depth list
    vector<list<int>>& get_depth_lists();
    //returns children list
    vector<list<int>>& get_children_list();
    //returns parent list
    vector<int>& get_parent_list();
    //returns object list
    vector<dbseg_seg_spatial_object<T>*>& get_object_list();
    //returns list of children at the level of the scanned image
    set<int> get_base_children(int o, int excluding = 0);
    //removes object from all aspects of the tree and reconnects tree links
    void destroy_object(int o);
    
    //returns all descendants of an object
    list<int>& get_descendants(list<int>&, int o);

    //returns list of existing objects
    list<int>& get_valid_IDs();
    
    void add_valid_ID(int o);

    void create_space();

protected:
    // a vector of each object by ID number that stores a list of each objects children by ID number
    vector<list<int>> children_list;
    // a vector of each object by ID number that stores the ID of its parent
    vector<int> parent_list;
    // a vector of each object
    vector<dbseg_seg_spatial_object<T>*> object_list;

    //the list of valid objects
    list<int> valid_IDs;


    // a vector by depth level of the lists of objects in each level
    vector<list<int>> depth_list;





};

#endif

