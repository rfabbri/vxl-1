// This is /lemsvxl/brcv/seg/dbdet/algo/dbdet_kovesi_edge_linker.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 1, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef DBDET_KOVESI_EDGE_LINKER_H_
#define DBDET_KOVESI_EDGE_LINKER_H_

#include<vcl_string.h>
#include<dbdet/edge/dbdet_edgemap_sptr.h>
#include<dbdet/sel/dbdet_curve_fragment_graph.h>

class dbdet_kovesi_edge_linker
{
private:
    vcl_string temp_dir_;
    vcl_string temp_cem_file_;
public:
    dbdet_kovesi_edge_linker(const vcl_string& temp_dir);
    ~dbdet_kovesi_edge_linker();
    void link_and_prune_edges(dbdet_edgemap_sptr& in_EM, int edge_threshold, int link_edge_length_threshold, dbdet_edgemap_sptr& out_EM, dbdet_curve_fragment_graph& out_CFG);
};

#endif /* DBDET_KOVESI_EDGE_LINKER_H_ */
