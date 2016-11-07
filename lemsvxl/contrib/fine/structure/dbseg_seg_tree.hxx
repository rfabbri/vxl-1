#ifndef dbseg_seg_tree_txx_
#define dbseg_seg_tree_txx_


#include "dbseg_seg_tree.h"


using namespace std;

template <class T>
dbseg_seg_tree<T>::dbseg_seg_tree(int numSegs) {
    list<int> l;
    dbseg_seg_spatial_object<T>* o;
    object_list.push_back(o);
    for (int i = 0; i < numSegs; i++) {
        children_list.push_back(l);
        parent_list.push_back(0);
    }
    /*dbseg_seg_tree::object_list = new vector<dbseg_seg_spatial_object*>;
    dbseg_seg_tree::children_list = new vector<list<int>>;
    dbseg_seg_tree::depth_list = new vector<list<int>>;
    dbseg_seg_tree::parent_list = new vector<int>;
    */
}

template <class T>
dbseg_seg_tree<T>::dbseg_seg_tree(std::list<int> validL, std::vector<dbseg_seg_spatial_object<T>*> objL, std::vector<int> parentL, std::vector<list<int> > childL, std::vector<list<int> > depthL) :
valid_IDs(validL),
object_list(objL),
parent_list(parentL),
children_list(childL),
depth_list(depthL)

{

}

template <class T>
dbseg_seg_tree<T>::~dbseg_seg_tree() {
    vector<dbseg_seg_spatial_object<T>*>::iterator i;
    for (i = object_list.begin(); i != object_list.end(); ++i) {
        delete *i;
    }
    /*
    dbseg_seg_tree::object_list->~vector();
    dbseg_seg_tree::children_list->~vector();
    dbseg_seg_tree::depth_list->~vector();
    dbseg_seg_tree::parent_list->~vector();
    */
    object_list.clear();
    children_list.clear();
    depth_list.clear();
    parent_list.clear();


}

template <class T>
bool dbseg_seg_tree<T>::add_child(int c, int p) {
    //tests if c is already a child of p
    list<int>::iterator i;
    int cs = children_list.size();
    if (children_list.size() <= p) {
        list<int> blankList;
        children_list.push_back(blankList);
    }
    for (i = (children_list)[p].begin(); i != (children_list)[p].end(); ++i) {
        if (*i == c) {
            return false;
        }

    }
    //adds as a child
    (children_list)[p].push_back(c);
    //sets the child to have the correct parent
    dbseg_seg_tree::set_parent(p, c);

    return true;
}

template <class T>
bool dbseg_seg_tree<T>::remove_child(int c, int p) {
    
    //tests if c is already a child of p
    list<int>::iterator i;
    for (i = (children_list)[p].begin(); i != (children_list)[p].end(); ++i) {
        if (*i == c) {
            //removes from list of children
            (dbseg_seg_tree::children_list)[p].remove(c);

            //changes the child's grandparent to its parent
            dbseg_seg_tree::set_parent(dbseg_seg_tree::get_parent(p), c);
            if (get_parent(c) != 0) { // if this does not become a top level object then add it as a child to it's parent
                add_child(c, get_parent(c));
            }
            //tests if this is the last child to see if the object needs to be removed
            if (dbseg_seg_tree::get_child_count(p) == 0) {
                dbseg_seg_tree::destroy_object(p);
                return false;
            }
            return true;
        }

    }

    return false;
}

template <class T>
void dbseg_seg_tree<T>::destroy_object(int o) {
    // if the object had a parent then remove it as a child
    if (dbseg_seg_tree::has_parent(o)) {
        dbseg_seg_tree::remove_child(o, (dbseg_seg_tree::parent_list)[o]);
    }
    (dbseg_seg_tree::children_list)[o].~list();
    (dbseg_seg_tree::depth_list)[(dbseg_seg_tree::object_list)[o]->get_depth()].remove(o);
    valid_IDs.remove(o);
    (dbseg_seg_tree::object_list)[o]->~dbseg_seg_spatial_object();

}

template <class T>
const int dbseg_seg_tree<T>::get_child_count(int o) {
    return (children_list)[o].size();
}

template <class T>
list<int>& dbseg_seg_tree<T>::get_children(int o) {
    return children_list[o];
}

template <class T>
vector<list<int>>& dbseg_seg_tree<T>::get_children_list() {
    return children_list;
}

template <class T>
vector<list<int>>& dbseg_seg_tree<T>::get_depth_lists() {
    return depth_list;
}

template <class T>
vector<dbseg_seg_spatial_object<T>*>& dbseg_seg_tree<T>::get_object_list() {
    return object_list;
}

template <class T>
const int dbseg_seg_tree<T>::get_parent(int o) {
    return (parent_list)[o];
}

template <class T>
vector<int>& dbseg_seg_tree<T>::get_parent_list() {
    return parent_list;
}

template <class T>
bool dbseg_seg_tree<T>::has_parent(int o) {
    return (parent_list)[o] > 0;
}

template <class T>
void dbseg_seg_tree<T>::set_parent(int p, int c) {
    if (parent_list.size() <= p) {
        parent_list.push_back(0);
    }
    (parent_list)[c]=p;
    //remove it from its current depth list
    (depth_list)[(object_list)[c]->get_depth()].remove(c);
    // if there is no parent then set depth to 1, otherwise set depth to one level below parent
    if (p==0) {
        (object_list)[c]->set_depth(1);
    }
    else {
        (object_list)[c]->set_depth((object_list)[p]->get_depth()+1);
    }
    //update depth lists
    if (depth_list.size() <= (object_list)[c]->get_depth()) {
        list<int> blankList;
        depth_list.push_back(blankList);
    }
    (depth_list)[(object_list)[c]->get_depth()].push_back(c);
    //update depths of children
    list<int>::iterator i;
    for (i = (children_list)[c].begin(); i != (children_list)[c].end(); ++i) {
        set_parent(c, *i);
    }    
}

template <class T>
set<int> dbseg_seg_tree<T>::get_base_children(int o, int excluding) {
    set<int> l;
    //if this is a base level object
    if (dbseg_seg_tree::get_child_count(o) == 0) {
        l.insert(o);
        return l;
    }
    //vector<list<int>> v;
    list<int>::iterator i;
    for (i = (dbseg_seg_tree::get_children_list())[o].begin(); i != (dbseg_seg_tree::get_children_list())[o].end(); ++i) {
        if (*i != excluding) {
            set<int> l2 = get_base_children(*i, excluding);
            set<int>::iterator j;
            for (j = l2.begin(); j != l2.end(); ++j) {
                l.insert(*j);
            }
        }
    }

    return l;

}

template <class T>
list<int>& dbseg_seg_tree<T>::get_valid_IDs() {
    return valid_IDs;
}

template <class T>
void dbseg_seg_tree<T>::add_valid_ID(int o) {
    valid_IDs.push_back(o);
}

template <class T>
void dbseg_seg_tree<T>::create_space() {
    list<int> blankList;
    children_list.push_back(blankList);
    parent_list.push_back(0);
}


template <class T>
list<int>& dbseg_seg_tree<T>::get_descendants(std::list<int> & l, int o) {
    l.push_back(o);
    list<int>::iterator i;
    for (i = children_list[o].begin(); i != children_list[o].end(); ++i) {
        get_descendants(l, *i);
    }
    return l;
}

#define DBSEG_SEG_TREE_INSTANTIATE(T) \
template class dbseg_seg_tree<T>;\

#endif // dbseg_seg_tree_txx_
