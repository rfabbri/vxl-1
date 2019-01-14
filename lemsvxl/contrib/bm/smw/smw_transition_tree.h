#ifndef SMW_TRANSITION_TREE_H_
#define SMW_TRANSITION_TREE_H_
//this is contrib/bm/smw/smw_transition_tree.h

#include<map>
#include<vector>

typedef unsigned transition_frequency;
typedef unsigned time_step;
typedef unsigned node_ids;
typedef unsigned table_ids;

typedef std::map<time_step,transition_frequency> time_map;
typedef std::map<node_ids,time_map> node_map;
typedef std::map<table_ids,node_map> transition_tree;


#endif //SMW_TRANSITION_TREE_H_
