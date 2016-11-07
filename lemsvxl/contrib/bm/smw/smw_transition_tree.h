#ifndef SMW_TRANSITION_TREE_H_
#define SMW_TRANSITION_TREE_H_
//this is contrib/bm/smw/smw_transition_tree.h

#include<vcl_map.h>
#include<vcl_vector.h>

typedef unsigned transition_frequency;
typedef unsigned time_step;
typedef unsigned node_ids;
typedef unsigned table_ids;

typedef vcl_map<time_step,transition_frequency> time_map;
typedef vcl_map<node_ids,time_map> node_map;
typedef vcl_map<table_ids,node_map> transition_tree;


#endif //SMW_TRANSITION_TREE_H_
