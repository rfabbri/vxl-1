// This is dbmsh3d/algo/dbmsh3d_graph_sh_path.h
// Ming-Ching Chang
// Oct 04, 2007.

#ifndef dbmsh3d_graph_sh_path_h_
#define dbmsh3d_graph_sh_path_h_

#include <dbmsh3d/dbmsh3d_mesh.h>

//: Find the graph shortest path on a mesh using Dijkstra.
//  Result:
//    - a map of Dist[vid, dist] for shortest distance for each vertex.
//    - a map of PrevV[vid, prevV] for backtracking from each vertex.
//
bool find_sh_path_on_M (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src,
                        vcl_map<int, float>& Dist,
                        vcl_map<int, dbmsh3d_vertex*>& PrevV,
                        const dbmsh3d_vertex* Dest = NULL);

bool find_shortest_Es_on_M (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src, const dbmsh3d_vertex* Dest,
                            vcl_vector<dbmsh3d_edge*>& Evec_path);

//###################################################################
//: Find the shortest path avoiding a given set of mesh edges/vertices.

bool find_sh_path_on_M_restrained (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src,
                                  vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                  vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                                  vcl_map<int, float>& Dist,
                                  vcl_map<int, dbmsh3d_vertex*>& PrevV,
                                  const dbmsh3d_vertex* Dest = NULL);

bool find_shortest_Es_on_M_restrained (dbmsh3d_mesh* M, const dbmsh3d_vertex* Src, const dbmsh3d_vertex* Dest,
                                       vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                       vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                                       vcl_vector<dbmsh3d_edge*>& Evec_path);

//###################################################################
//: Find the shortest path with restrains:
//    1) avoiding a given set of mesh edges/vertices.
//    2) search from a set of source vertices.
//    3) stop if any given destination vertex is reached.
//

bool find_sh_path_on_M_restrained_targets (dbmsh3d_mesh* M, 
                                           vcl_set<dbmsh3d_vertex*>& src_set,
                                           vcl_set<dbmsh3d_vertex*>& dest_set,
                                           vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                           vcl_set<dbmsh3d_vertex*>& avoid_Vset,
                                           vcl_map<int, float>& Dist,
                                           vcl_map<int, dbmsh3d_vertex*>& PrevV,
                                           dbmsh3d_vertex*& destV);

bool find_shortest_Es_on_M_restrained_targets (dbmsh3d_mesh* M, 
                                               vcl_set<dbmsh3d_vertex*>& src_set, 
                                               vcl_set<dbmsh3d_vertex*>& dest_set,
                                               vcl_set<dbmsh3d_edge*>& avoid_Eset, 
                                               vcl_set<dbmsh3d_vertex*>& avoid_Vset, 
                                               vcl_vector<dbmsh3d_edge*>& Evec_path, 
                                               dbmsh3d_vertex*& srcV, dbmsh3d_vertex*& destV);

#endif //dbmsh3d_graph_sh_path_h_



