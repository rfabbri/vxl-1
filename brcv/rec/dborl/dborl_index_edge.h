//:
// \file
// \brief  Just a dummy class to be able to use dbgrl_graph for the index. No data is saved in the index edges. 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#if !defined(_DBORL_INDEX_EDGE_H)
#define _DBORL_INDEX_EDGE_H

#include <dborl/dborl_index_node_base.h>
#include <dborl/dborl_index_node_base_sptr.h>
#include <dbgrl/dbgrl_edge.h>

class dborl_index_edge : public dbgrl_edge<dborl_index_node_base> 
{
public:

  dborl_index_edge(dborl_index_node_base_sptr v1, dborl_index_node_base_sptr v2) : dbgrl_edge<dborl_index_node_base>(v1, v2) {};
};

#endif  //_DBORL_INDEX_EDGE_H
