// This is file shp/dbsk2d/Templates/vtol_list_functions+dbsk2d_bnd_edge~-.cxx

#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <vtol/vtol_list_functions.txx>
#include <vcl_vector.txx>

template vcl_vector<dbsk2d_bnd_edge* >* tagged_union(vcl_vector<dbsk2d_bnd_edge*>*);

template vcl_vector<dbsk2d_bnd_edge_sptr >* tagged_union(vcl_vector<dbsk2d_bnd_edge_sptr>*);

template vcl_list<dbsk2d_bnd_edge* >* tagged_union(vcl_list<dbsk2d_bnd_edge* >*);

template vcl_list<dbsk2d_bnd_edge_sptr >* tagged_union(vcl_list<dbsk2d_bnd_edge_sptr >*);






