// This is file shp/dbsk2d/Templates/vtol_list_functions+dbsk2d_bnd_vertex~-.cxx

#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>

#include <vtol/vtol_list_functions.hxx>
#include <vcl_vector.hxx>
#include <vcl_list.hxx>

template vcl_vector<dbsk2d_bnd_vertex* >* tagged_union(vcl_vector<dbsk2d_bnd_vertex*>*);

template vcl_vector<dbsk2d_bnd_vertex_sptr >* tagged_union(vcl_vector<dbsk2d_bnd_vertex_sptr>* );

template vcl_list<dbsk2d_bnd_vertex* >* tagged_union(vcl_list<dbsk2d_bnd_vertex*>*);

template vcl_list<dbsk2d_bnd_vertex_sptr >* tagged_union(vcl_list<dbsk2d_bnd_vertex_sptr>* );


