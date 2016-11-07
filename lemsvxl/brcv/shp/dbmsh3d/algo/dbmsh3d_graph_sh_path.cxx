// This is dbmsh3d/algo/dbmsh3d_graph_sh_path.cxx
// Ming-Ching Chang
// Oct 04, 2007.

#include <vcl_queue.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>

//: find the graph shortest path on a mesh using Dijkstra.
//    - specify the destination vertex Dest for early-jumpout.
//  Result:
//    - a map of Dist[vid, dist] for shortest distance for each vertex.
//    - a map of PrevV[vid, prevV] for backtracking from each vertex.
//
bool find_sh_path_on_M (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src,
                        vcl_map<int, float>& Dist,
                        vcl_map<int, dbmsh3d_vertex*>& PrevV,
                        const dbmsh3d_vertex* Dest)
{
  //1) Check if input is valid.
  assert (M->is_MHE());
  if (M->vertexmap (Src->id()) != Src)
    return false;
  if (Dest) {
    if (M->vertexmap (Dest->id()) != Dest)
      return false;
  }

  //2) Initialization:
  // - reset all V's dist to infinity, set all V to unvisited.
  assert (Dist.size() == 0);
  assert (PrevV.size() == 0);
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_i_visited (0);
    Dist.insert (vcl_pair<int, float> (V->id(), FLT_MAX));
    PrevV.insert (vcl_pair<int, dbmsh3d_vertex*> (V->id(), NULL));
  }
  assert (Dist.size() == M->vertexmap().size());
  assert (PrevV.size() == M->vertexmap().size());

  // - initialize the Src vertex.
  Dist[Src->id()] = 0;

  // - initilize the priority queue PW <-dist, V>.
  vcl_priority_queue<vcl_pair<float, dbmsh3d_vertex*> > PQ;
  PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[Src->id()], (dbmsh3d_vertex*) Src));

  //3) Main Loop:
  while (PQ.size() != 0) {
    //Remove the top vertex U from PQ.
    dbmsh3d_vertex* U = PQ.top().second;
    PQ.pop ();

    //Skip if U is visited.
    //Visit U (the current closest vertex).
    //U's dist and prevV is finalized as its current value.
    if (U->is_visited())
      continue;
    U->set_i_visited (true);

    //If Dest is specified, check if U == Dest for early-jumpout.
    if (U == Dest)
      return true;

    //For each neighbor V of U, compute new_dist = dist[U] + d_UV.
    // - if new_dist < dist[V], update dist[V] and set prev[V] = U.
    // - add V to PQ.
    for (dbmsh3d_ptr_node* cur = U->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      dbmsh3d_vertex* V = E->other_V (U);
      if (V->is_visited())
        continue; //Skip if V is visited.

      double d = Dist[U->id()] + vgl_distance (U->pt(), V->pt());
      if (d < Dist[V->id()]) {
        Dist[V->id()] = float (d);
        PrevV[V->id()] = U;
        PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[V->id()], V));
      }
    }
  }

  if (Dest) 
    return false; //Dest is not found after search is done.
  else
    return true;
}
                        
bool find_shortest_Es_on_M (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src, const dbmsh3d_vertex* Dest,
                            vcl_vector<dbmsh3d_edge*>& Evec_path)
{  
  vcl_map<int, float> Dist;
  vcl_map<int, dbmsh3d_vertex*> PrevV;

  bool result = find_sh_path_on_M (M, Src, Dist, PrevV, Dest);
  if (result == false)
    return false;

  //Back track mesh edges from Dest to Src into Evec_path.
  assert (Evec_path.size() == 0);
  dbmsh3d_vertex* V = (dbmsh3d_vertex*) Dest;
  while (V != Src) {
    dbmsh3d_vertex* pV = PrevV[V->id()];
    assert (pV);
    dbmsh3d_edge* E = E_sharing_2V (V, pV);
    Evec_path.push_back (E);
    V = pV;
  }

  return true;
}

//###################################################################

//: A special version of shortest path avoiding set of mesh edges.

bool find_sh_path_on_M_restrained (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src,
                          vcl_set<dbmsh3d_edge*>& avoid_Eset,
                          vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                          vcl_map<int, float>& Dist,
                          vcl_map<int, dbmsh3d_vertex*>& PrevV,
                          const dbmsh3d_vertex* Dest)
{  
  //1) Check if input is valid.
  assert (M->is_MHE());
  if (M->vertexmap (Src->id()) != Src)
    return false;
  assert (avoid_Vset.find ((dbmsh3d_vertex*)Src) == avoid_Vset.end());
  if (Dest) {
    if (M->vertexmap (Dest->id()) != Dest)
      return false;
  }  

  //2) Initialization:
  // - reset all V's dist to infinity, set all V to unvisited.
  assert (Dist.size() == 0);
  assert (PrevV.size() == 0);
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_i_visited (0);
    Dist.insert (vcl_pair<int, float> (V->id(), FLT_MAX));
    PrevV.insert (vcl_pair<int, dbmsh3d_vertex*> (V->id(), NULL));
  }
  assert (Dist.size() == M->vertexmap().size());
  assert (PrevV.size() == M->vertexmap().size());

  // - initialize the Src vertex.
  Dist[Src->id()] = 0;

  // - initilize the priority queue PW <-dist, V>.
  vcl_priority_queue<vcl_pair<float, dbmsh3d_vertex*> > PQ;
  PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[Src->id()], (dbmsh3d_vertex*) Src));

  //3) Main Loop:
  while (PQ.size() != 0) {
    //Remove the top vertex U from PQ.
    dbmsh3d_vertex* U = PQ.top().second;
    PQ.pop ();

    //Skip if U is visited.
    //Visit U (the current closest vertex).
    //U's dist and prevV is finalized as its current value.
    if (U->is_visited())
      continue;
    U->set_i_visited (true);

    //If Dest is specified, check if U == Dest for early-jumpout.
    if (U == Dest)
      return true;

    if (avoid_Vset.find(U) != avoid_Vset.end())
      continue; //Stop propagation if U is in avoid_Vset.

    //For each neighbor V of U, compute new_dist = dist[U] + d_UV.
    // - if new_dist < dist[V], update dist[V] and set prev[V] = U.
    // - add V to PQ.
    for (dbmsh3d_ptr_node* cur = U->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      if (avoid_Eset.find (E) != avoid_Eset.end())
        continue; //Stop propagation if E is in avoid_Eset.
      dbmsh3d_vertex* V = E->other_V (U);
      if (V->is_visited())
        continue; //Skip if V is visited.

      double d = Dist[U->id()] + vgl_distance (U->pt(), V->pt());
      if (d < Dist[V->id()]) {
        Dist[V->id()] = float (d);
        PrevV[V->id()] = U;
        PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[V->id()], V));
      }
    }
  }

  if (Dest) 
    return false; //Dest is not found after search is done.
  else
    return true;
}

bool find_shortest_Es_on_M_restrained (dbmsh3d_mesh* M, 
                                       const dbmsh3d_vertex* Src, const dbmsh3d_vertex* Dest,
                                       vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                       vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                                       vcl_vector<dbmsh3d_edge*>& Evec_path)
{
  vcl_map<int, float> Dist;
  vcl_map<int, dbmsh3d_vertex*> PrevV;
  if (Dest == NULL)
    return false; //No destination specified.
  bool result = find_sh_path_on_M_restrained (M, Src, avoid_Eset, avoid_Vset, Dist, PrevV, Dest);
  if (result == false)
    return false;

  //Back track mesh edges from Dest to Src into Evec_path.
  assert (Evec_path.size() == 0);
  dbmsh3d_vertex* V = (dbmsh3d_vertex*) Dest;
  while (V != Src) {
    dbmsh3d_vertex* pV = PrevV[V->id()];
    assert (pV);
    dbmsh3d_edge* E = E_sharing_2V (V, pV);
    Evec_path.push_back (E);
    V = pV;
  }

  return true;
}

//###################################################################
//: Find the shortest path with restrains:
//    1) avoiding a given set of mesh edges/vertices.
//    2) search from a set of source vertices.
//    3) stop if any given destination vertex is reached.

bool find_sh_path_on_M_restrained_targets (dbmsh3d_mesh* M, 
                                           vcl_set<dbmsh3d_vertex*>& src_set,
                                           vcl_set<dbmsh3d_vertex*>& dest_set,
                                           vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                           vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                                           vcl_map<int, float>& Dist,
                                           vcl_map<int, dbmsh3d_vertex*>& PrevV,
                                           dbmsh3d_vertex*& destV)
{
  //1) Check if input is valid.
  assert (M->is_MHE());
  vcl_set<dbmsh3d_vertex*>::iterator it = src_set.begin();
  for (; it != src_set.end(); it++) {
    dbmsh3d_vertex* src = (*it);
    assert (avoid_Vset.find (src) == avoid_Vset.end());
    if (M->vertexmap (src->id()) != src)
      return false;
  }
  if (dest_set.empty() == false) {
    it = dest_set.begin();
    for (; it != dest_set.end(); it++) {
      dbmsh3d_vertex* dest = (*it);
      if (M->vertexmap (dest->id()) != dest)
        return false;
    }
  }  

  //2) Initialization:
  // - reset all V's dist to infinity, set all V to unvisited.
  assert (Dist.size() == 0);
  assert (PrevV.size() == 0);
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_i_visited (0);
    Dist.insert (vcl_pair<int, float> (V->id(), FLT_MAX));
    PrevV.insert (vcl_pair<int, dbmsh3d_vertex*> (V->id(), NULL));
  }
  assert (Dist.size() == M->vertexmap().size());
  assert (PrevV.size() == M->vertexmap().size());
  destV = NULL;

  // - initilize the priority queue PW <-dist, V>.
  // - initialize the Src vertices.
  vcl_priority_queue<vcl_pair<float, dbmsh3d_vertex*> > PQ;
  it = src_set.begin();
  for (; it != src_set.end(); it++) {
    dbmsh3d_vertex* src = (*it);
    Dist[src->id()] = 0;
    PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[src->id()], src));
  }  

  //3) Main Loop:
  while (PQ.size() != 0) {
    //Remove the top vertex U from PQ.
    dbmsh3d_vertex* U = PQ.top().second;
    PQ.pop ();

    //Skip if U is visited.
    //Visit U (the current closest vertex).
    //U's dist and prevV is finalized as its current value.
    if (U->is_visited())
      continue;
    U->set_i_visited (true);

    //If Dest is specified, check if U in Dest for early-jumpout.
    if (dest_set.find (U) != dest_set.end()) {
      destV = U;
      return true;
    }

    if (avoid_Vset.find(U) != avoid_Vset.end())
      continue; //Stop propagation if U is in avoid_Vset.

    //For each neighbor V of U, compute new_dist = dist[U] + d_UV.
    // - if new_dist < dist[V], update dist[V] and set prev[V] = U.
    // - add V to PQ.
    for (dbmsh3d_ptr_node* cur = U->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      if (avoid_Eset.find (E) != avoid_Eset.end())
        continue; //Stop propagation if E is in avoid_Eset.
      dbmsh3d_vertex* V = E->other_V (U);
      if (V->is_visited())
        continue; //Skip if V is visited.

      double d = Dist[U->id()] + vgl_distance (U->pt(), V->pt());
      if (d < Dist[V->id()]) {
        Dist[V->id()] = float (d);
        PrevV[V->id()] = U;
        PQ.push (vcl_pair<float, dbmsh3d_vertex*> (-Dist[V->id()], V));
      }
    }
  }

  if (dest_set.empty() == false) 
    return false; //Dest is not found after search is done.
  else
    return true;
}

bool find_shortest_Es_on_M_restrained_targets (dbmsh3d_mesh* M, 
                                               vcl_set<dbmsh3d_vertex*>& src_set, 
                                               vcl_set<dbmsh3d_vertex*>& dest_set,
                                               vcl_set<dbmsh3d_edge*>& avoid_Eset, 
                                               vcl_set<dbmsh3d_vertex*>& avoid_Vset, 
                                               vcl_vector<dbmsh3d_edge*>& Evec_path, 
                                               dbmsh3d_vertex*& srcV, dbmsh3d_vertex*& destV)
{
  vcl_map<int, float> Dist;
  vcl_map<int, dbmsh3d_vertex*> PrevV;

  if (dest_set.empty())
    return false; //No destination specified.

  bool result = find_sh_path_on_M_restrained_targets (M, src_set, dest_set, avoid_Eset, avoid_Vset, Dist, PrevV, destV);
  if (result == false)
    return false;

  //Back track mesh edges from Dest to Src into Evec_path.
  assert (Evec_path.size() == 0);
  dbmsh3d_vertex* V = destV;
  while (src_set.find (V) == src_set.end()) {
    dbmsh3d_vertex* pV = PrevV[V->id()];
    assert (pV);
    dbmsh3d_edge* E = E_sharing_2V (V, pV);
    Evec_path.push_back (E);
    V = pV;
  }
  srcV = V;

  return true;

}


