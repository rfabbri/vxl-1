// This is file shp/dbsk2d/Templates/vtol_list_functions+dbsk2d_bnd_vertex~-.cxx

#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>

#include <vtol/vtol_list_functions.hxx>
#include <vector>
#include <list>

template std::vector<dbsk2d_bnd_vertex* >* tagged_union(std::vector<dbsk2d_bnd_vertex*>*);

template std::vector<dbsk2d_bnd_vertex_sptr >* tagged_union(std::vector<dbsk2d_bnd_vertex_sptr>* );

template std::list<dbsk2d_bnd_vertex* >* tagged_union(std::list<dbsk2d_bnd_vertex*>*);

template std::list<dbsk2d_bnd_vertex_sptr >* tagged_union(std::list<dbsk2d_bnd_vertex_sptr>* );


