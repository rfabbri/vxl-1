#ifndef dbmsh3d_sheet_algo_h_
#define dbmsh3d_sheet_algo_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_sheet_algo.h
//:
// \file
// \brief  dbmsh3d sheet related algorithms.
//
//
// \author
//  MingChing Chang  Sep 10, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_sheet.h>

#include <dbmsh3d/dbmsh3d_hypg.h>

//: Merge two adjacent sheets sharing an edge E.
//  Delete edge E and connect the boundary chain of S2 to S1.
bool merge_sheets_sharing_E (dbmsh3d_hypg* H, dbmsh3d_edge* E, 
                             dbmsh3d_sheet* S1, dbmsh3d_sheet* S2,
                             const bool delete_E);


//###### Sheet Topology in Merging /Splitting ######

//: Given a starting N and icurve C, find the breaking i-curves till the end.
void get_S_icurve_vec_otherN (dbmsh3d_sheet* S, dbmsh3d_vertex* startN, dbmsh3d_edge* startC, 
                              vcl_vector<dbmsh3d_edge*>& icurve_otherN_N,
                              dbmsh3d_vertex*& otherN);

//: Given a starting N and icurve C, find the breaking i-curves till the end.
//  Return false if the found bndN has more than 2 icurve or bnd_chain incident to it!
bool get_S_icurve_vec_bndN (dbmsh3d_sheet* S, dbmsh3d_vertex* startN, dbmsh3d_edge* startC,
                            vcl_vector<vcl_vector<dbmsh3d_edge*> >& icurve_bndN_N,
                            vcl_vector<dbmsh3d_edge*>& icurve_loop_E_heads,
                            dbmsh3d_vertex*& bndN);


#endif

