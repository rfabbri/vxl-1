// This is brcv/shp/dbsk2d/dbsk2d_ishock_utils.h
#ifndef dbsk2d_ishock_utils_h_
#define dbsk2d_ishock_utils_h_
//:
// \file
// \brief This file contains various useful functions 
// for use internal to intrinsic shock classes
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_utils.h>

#include <vector>
#include <list>
#include <map>
#include <utility>

//forward declaration
class dbsk2d_ishock_elm;
class dbsk2d_ishock_node;
class dbsk2d_ishock_edge;

// Useful type definitions

typedef std::list<dbsk2d_ishock_node* > ishock_node_list;
typedef ishock_node_list::iterator ishock_node_list_iter;

typedef std::list<dbsk2d_ishock_edge* > ishock_edge_list;
typedef ishock_edge_list::iterator ishock_edge_list_iter;

//: typedefs for ordered active shock list
typedef std::pair<double, int> r_id_pair; //id is necessary for deletions
typedef std::pair<r_id_pair, dbsk2d_ishock_edge*> key_selm_pair;

typedef std::multimap<r_id_pair, dbsk2d_ishock_edge*> ordered_shock_list; //Radius(=simTime) & id
typedef ordered_shock_list::const_iterator ordered_shock_list_iter;
typedef ordered_shock_list::const_reverse_iterator ordered_shock_list_riter;

typedef std::pair<int, dbsk2d_ishock_elm*> id_ishock_elm_pair;

typedef std::multimap<double, dbsk2d_ishock_node*> ordered_src_list;
typedef ordered_src_list::const_iterator ordered_src_list_iter;
typedef std::pair<double, dbsk2d_ishock_node*> time_src_pair;


#endif //dbsk2d_ishock_utils_h_
