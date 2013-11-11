// This is /lemsvxl/contrib/firat/xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jul 1, 2010
//

#include "dbsksp_convert_xgraph_to_binary_mask_utils.h"
#include <vcl_stack.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_graph.h>

bool dbsksp_draw_binary_map(const dbsksp_xshock_graph_sptr& xgraph, vil_image_view<vxl_byte >& screenshot)
{
    if (!xgraph) return false;
    if (screenshot.size() == 0) return false;


    int inner_radius = 1;

    for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
            xgraph->edges_end(); ++eit)
    {
        dbsksp_xshock_edge_sptr xe = *eit;
        dbsksp_xshock_node_descriptor start = *xe->source()->descriptor(xe);
        dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();
        dbsksp_xshock_fragment xfrag(start, end);

        dbgl_biarc bnd[] = {xfrag.bnd_left_as_biarc(), xfrag.bnd_right_as_biarc()};
        for (int i=0; i < 2; ++i)
        {
            dbgl_biarc biarc = bnd[i];
            double len = biarc.len();
            for (double s = 0; s <= len; s = s+.05)
            {
                vgl_point_2d<double > p = biarc.point_at(s);
                vgl_vector_2d<double > t = biarc.tangent_at(s);
                vgl_vector_2d<double > n(-t.y(), t.x());

                // inner color
                for (int k = 0; k <= 0; ++k)
                {
                    vgl_point_2d<double > pt = p + k * n;
                    vgl_point_2d<int > pi( int(pt.x()), int(pt.y()));

                    if (screenshot.in_range(pi.x(), pi.y()))
                    {
                        screenshot(pi.x(), pi.y(), 0) = 255;
                    }
                } // for
            }
        }
    }
    vcl_stack<int> xs, ys;

    xs.push(xgraph->all_nodes().front()->pt().x());
    ys.push(xgraph->all_nodes().front()->pt().y());
    while(!xs.empty())
    {
        int x = xs.top();
        int y = ys.top();
        xs.pop();
        ys.pop();
        screenshot(x,y) = 255;
        if(screenshot.in_range(x,y-1) && screenshot(x,y-1) == 0)
        {
            xs.push(x);
            ys.push(y-1);
        }
        if(screenshot.in_range(x+1,y) &&screenshot(x+1,y) == 0)
        {
            xs.push(x+1);
            ys.push(y);
        }
        if(screenshot.in_range(x,y+1) &&screenshot(x,y+1) == 0)
        {
            xs.push(x);
            ys.push(y+1);
        }
        if(screenshot.in_range(x-1,y) &&screenshot(x-1,y) == 0)
        {
            xs.push(x-1);
            ys.push(y);
        }
    }
    return true;
}
