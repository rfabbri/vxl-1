#include "dbseg_seg_neighborhood.h"
using namespace std;
dbseg_seg_neighborhood::dbseg_seg_neighborhood(int numSegs) {
    //dbseg_seg_neighborhood::neighbor_list = new vector<list<int>>(4);
    list<int> l;
    for (int i = 0; i < numSegs; i++) {
        neighbor_list.push_back(l);
    }

}

dbseg_seg_neighborhood::dbseg_seg_neighborhood(vector<list<int>> listL): neighbor_list(listL) {
}
    

dbseg_seg_neighborhood::~dbseg_seg_neighborhood() {

}

void dbseg_seg_neighborhood::add_neighbor(int n, int home) {
    //creates room for the home if it isn't in the vector already
    if (dbseg_seg_neighborhood::neighbor_list.size() <= home) {
        neighbor_list.resize(home+1);
    }
    if (dbseg_seg_neighborhood::neighbor_list.size() <= n) {
        neighbor_list.resize(n+1);
    }
    /*
    if (neighbor_list[home].size() < d) {
        neighbor_list[home].resize(d);
    }*/

    //tests if n is already a neighbor of home
    /*list<int> blankList;
    if (neighbor_list.size() <= home) {
        neighbor_list.push_back(blankList);
    }*/
    bool add = true;
    list<int>::iterator i;
    for (i = neighbor_list[home].begin(); i != neighbor_list[home].end(); ++i) {
        if (*i == n) {
            add = false;
            break;
        }

    }
    if (add) {
        neighbor_list[home].push_back(n);
    }
    add = true;
    //list<int>::iterator i;
    for (i = neighbor_list[n].begin(); i != neighbor_list[n].end(); ++i) {
        if (*i == home) {
            add = false;
            break;
        }

    }
    if (add) {
        neighbor_list[n].push_back(home);
    }

    return;
}

void dbseg_seg_neighborhood::remove_neighbor(int n, int home) {
    list<int>::iterator i;
    for (i = neighbor_list[home].begin(); i != neighbor_list[home].end(); ++i) {
        if (*i == n) {
            neighbor_list[home].remove(n);
            break;
        }

    }

    for (i = neighbor_list[n].begin(); i != neighbor_list[n].end(); ++i) {
        if (*i == home) {
            neighbor_list[n].remove(home);
            break;
        }

    }
    return;

}

void dbseg_seg_neighborhood::clear_neighbors(int o) {
    list<int>::iterator i;
    for (i = neighbor_list[o].begin(); i != neighbor_list[o].end(); ++i) {
        neighbor_list[*i].remove(o);
    }
    neighbor_list[o].clear();
}


vector<list<int>>& dbseg_seg_neighborhood::get_neighbor_list() {
    return neighbor_list;
}

list<int>& dbseg_seg_neighborhood::get_neighbors(int o) {
    return neighbor_list[o];
}

void dbseg_seg_neighborhood::add_no_neighbors(int o) {
    while(neighbor_list.size() <= o) {
        list<int> blankList;
        neighbor_list.push_back(blankList);
    }
}

bool dbseg_seg_neighborhood::is_neighbors(int o1, int o2) {
    list<int>::iterator i;
    list<int> tempList;
    if (neighbor_list[o1].size() > neighbor_list[o2].size()) {
        tempList = neighbor_list[o2];
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            if (*i == o1) {
                return true;
            }
        }
    }
    else {
        tempList = neighbor_list[o1];
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            if (*i == o2) {
                return true;
            }
        }
    }
    return false;
}


void dbseg_seg_neighborhood::create_space() {
    list<int> blankList;
    neighbor_list.push_back(blankList);
}

