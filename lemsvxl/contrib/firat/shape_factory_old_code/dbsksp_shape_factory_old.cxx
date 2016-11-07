// This is /lemsvxl/contrib/firat/shape_factory/dbsksp_shape_factory.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jun 27, 2010
//


#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <dbsksp/pro/dbsksp_load_xgraph_xml_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vcl_string.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/vis/dbsksp_xshock_design_tool.h>
#include <dbsksp/algo/dbsksp_screenshot.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsks/algo/dbsks_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_stack.h>

// ARGS
// 1: xgraph geom file
// 2: xgraph file
// 3: img width
// 4: img height
// 5: initially selected node
// 6: log2-scale factor


//------------------------------------------------------------------------------
//: Draw the shock graph on top of an existing image
bool dbsksp_draw_binary_map(const dbsksp_xshock_graph_sptr& xgraph,
        vil_image_view<vxl_byte >& screenshot)
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
    xs.push(screenshot.ni()/2);
    ys.push(screenshot.nj()/2);
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

void save_xgraph_as_image(dbsksp_xgraph_storage_sptr& shock_storage, vil_image_view<vxl_byte >& bg_image, const vcl_string& file_path)
{
    dbsksp_xshock_graph_sptr xg = shock_storage->xgraph();
    //vil_image_view<vxl_byte > screenshot_image;
    vil_rgb<vxl_byte> contour_color(0, 0, 255); // blue
    vil_rgb<vxl_byte> padding_color(255, 255, 0); // yellow
    //dbsksp_screenshot(bg_image, xg, screenshot_image, 1, 1, contour_color, padding_color);
    dbsksp_draw_binary_map(xg, bg_image);
    vil_save(bg_image, file_path.c_str());
}

int main(int argn, char* args[])
{
    vcl_cout << "Number of input arguments: " << argn <<vcl_endl;
    if(argn != 7)
    {
        vcl_cout << "Not enough inputs!" <<vcl_endl;
        return 0;
    }

    vcl_cout << "Xgraph geom file path: " << args[1] << vcl_endl;
    vcl_cout << "Xgraph file path: " << args[2] << vcl_endl;

    vcl_string xgraph_geom_file = args[1];
    vcl_string xgraph_file = args[2];
    int width = atoi(args[3]);
    int height = atoi(args[4]);
    int selected_node_id = atoi(args[5]);
    double scale_factor = atof(args[6]);


    dbsksp_xshock_graph_sptr xg = 0;
    x_read(xgraph_file, xg);
    dbsksp_xgraph_storage_sptr shock_storage = dbsksp_xgraph_storage_new();
    shock_storage->set_xgraph(xg);
    vcl_cout << "Loading xshock_graph XML file completed.\n";
    dbsksp_xshock_design_tool dt;
    dt.set_storage(shock_storage);

    dbsks_xgraph_geom_model_sptr geom_model = 0;
    //dbsks_load_xgraph_geom_model(xgraph_geom_file, geom_model);

    vil_image_resource_sptr image_resource = vil_new_image_resource(width, height, 1,  VIL_PIXEL_FORMAT_BYTE);
    vil_image_view<vxl_byte > bg_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());
    bg_image.fill(0);

    dbsksp_xshock_node_sptr selected_node = xg->node_from_id(selected_node_id);
    double x_coor = selected_node->pt().x();
    double y_coor = selected_node->pt().y();
    vcl_cout << "Selected node position: " << x_coor << " " << y_coor << vcl_endl;
    dt.set_selected_xnode(selected_node);
    dt.handle_similarity_transform(-x_coor+width/2, -y_coor+height/2, 0, scale_factor);

    int num_nodes = xg->number_of_vertices();
    vcl_cout << "Number of nodes: " << num_nodes << vcl_endl;
    int num_edges = xg->number_of_edges();
    vcl_cout << "Number of edges: " << num_edges << vcl_endl;


    /*int* vertex_ids = new int[num_nodes];
    int count = 0;
    int index = 1;
    while(count != num_nodes)
    {
        if(xg->node_from_id(index))
        {
            vertex_ids[count++] = index;
        }
        index++;
    }

    for(int i = 0; i < num_nodes; i++)
    {
        dbsksp_xshock_node_sptr current_node = xg->node_from_id(vertex_ids[i]);

        dt.set_selected_xnode(current_node);
        for(int j = 0; j < current_node->edge_list().size(); j++)
        {
            dt.set_selected_xedge(current_node->edge_list().front());

        }

    }*/



    save_xgraph_as_image(shock_storage, bg_image, "/home/firat/Desktop/mod2-xml.png");
    //delete[] vertex_ids;
    return 0;
}
