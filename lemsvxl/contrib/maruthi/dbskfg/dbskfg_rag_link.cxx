// This is brcv/shp/dbskfg/dbskfg_rag_link.cxx

//:
// \file

#include <dbskfg/dbskfg_rag_link.h>


dbskfg_rag_link::dbskfg_rag_link
( 

    unsigned int id

):dbgrl_edge<dbskfg_rag_node>(),
  id_(id)
{

}

dbskfg_rag_link::dbskfg_rag_link
(dbskfg_rag_node_sptr source,
 dbskfg_rag_node_sptr target,
 unsigned int id
)
    :dbgrl_edge<dbskfg_rag_node>(source,target),
     id_(id)
{

}


