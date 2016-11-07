// This is /lemsvxl/contrib/firat/shape_factory/dbsksp_iterative_averaging.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Aug 2, 2010
//

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>

double compute_edit_distance2(const dbsksp_xshock_graph_sptr& xgraph1,
                      const dbsksp_xshock_graph_sptr& xgraph2,
                      dbsksp_edit_distance& work)
{
  dbsksp_edit_distance edit_distance;
  edit_distance.set(xgraph1, xgraph2, 6, 3);
  edit_distance.save_path(true);
  edit_distance.edit();
  work = edit_distance;
  return edit_distance.final_cost();
}




int main()
{
    dbsksp_xshock_graph_sptr xg1 = 0, xg2 = 0;
    x_read("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/swans-xgraph/swans_aal.xgraph.0.xml", xg1);
    x_read("/home/firat/Desktop/swan-mask.png/swan.xml", xg2);
    dbsksp_edit_distance work;
    double cost = compute_edit_distance2(xg1,xg2, work);
    dbsksp_xshock_graph_sptr xg3 = work.edit_xgraph2_to_common_topology();
    vcl_vector<pathtable_key > work_corr;
    work.get_final_correspondence(work_corr);
    for(int i = 0; i < work_corr.size(); i++)
    {
        pathtable_key k = work_corr[i];

    }
    xg3->edge_from_id(9)->set_id(100);
    xg3->node_from_id(1)->set_id(666);
    x_write("/home/firat/Desktop/swan-mask.png/swan-89.xml", xg3);



}
