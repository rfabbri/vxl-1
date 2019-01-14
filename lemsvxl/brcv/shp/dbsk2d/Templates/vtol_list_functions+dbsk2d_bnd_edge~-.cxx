// This is file shp/dbsk2d/Templates/vtol_list_functions+dbsk2d_bnd_edge~-.cxx

#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <vtol/vtol_list_functions.hxx>
#include <vector>

template std::vector<dbsk2d_bnd_edge* >* tagged_union(std::vector<dbsk2d_bnd_edge*>*);

template std::vector<dbsk2d_bnd_edge_sptr >* tagged_union(std::vector<dbsk2d_bnd_edge_sptr>*);

template std::list<dbsk2d_bnd_edge* >* tagged_union(std::list<dbsk2d_bnd_edge* >*);

template std::list<dbsk2d_bnd_edge_sptr >* tagged_union(std::list<dbsk2d_bnd_edge_sptr >*);






