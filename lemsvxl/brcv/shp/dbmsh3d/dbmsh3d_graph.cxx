//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_hypermesh.cxx
//:
// \file
// \brief Graph
//
//
// \author
//  MingChing Chang  July 05, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_graph.h>

//###############################################################
//    GRAPH
//###############################################################

////////////////////////////////////////////////////////////////////
//: Validation & Report

bool dbmsh3d_graph::check_integrity ()
{
  if (dbmsh3d_pt_set::check_integrity() == false)
    return false;
  
  vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    dbmsh3d_edge* E = (*it).second;
    if (E->id() != (*it).first)
      return false;
    if (E->check_integrity() == false)
      return false;
  }

  return true;
}

void dbmsh3d_graph::summary_report ()
{
}

//: Remove all loops of the input graph G.
void remove_graph_loops (dbmsh3d_graph* G)
{
  vul_printf (vcl_cout, "remove_graph_loops():\n");
  unsigned int count = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator it = G->edgemap().begin();
  while (it != G->edgemap().end()) { 
    dbmsh3d_edge* E = (*it).second;    
    if (E->is_self_loop()) {
      vul_printf (vcl_cout, "\tremoving graph edge %d.\n", E->id());
      vcl_map<int, dbmsh3d_edge*>::iterator tmp = it;
      tmp++;
      count++;
      G->remove_edge (E);
      it = tmp;
    }
    else
      it++;
  }
  vul_printf (vcl_cout, "\tdone. %u loop(s) removed.\n", count);
}

void clone_graph (dbmsh3d_graph* targetG, dbmsh3d_graph* inputG)
{
  clone_ptset (targetG, inputG);

  //Clone all graph edges.
  vcl_map<int, dbmsh3d_edge*>::iterator it = inputG->edgemap().begin();
  for (; it != inputG->edgemap().end(); it++) {
    dbmsh3d_edge* E = (*it).second;
    dbmsh3d_vertex* sV = E->vertices(0);
    dbmsh3d_vertex* eV = E->vertices(1);
    dbmsh3d_vertex* newSV = targetG->vertexmap (sV->id());
    dbmsh3d_vertex* newEV = targetG->vertexmap (eV->id());
    dbmsh3d_edge* newE = targetG->_new_edge (newSV, newEV, E->id());
    targetG->add_edge_incidence_check (newE); //faster version: add_edge_incidence()
  }
  targetG->set_edge_id_counter (inputG->edge_id_counter());
}



