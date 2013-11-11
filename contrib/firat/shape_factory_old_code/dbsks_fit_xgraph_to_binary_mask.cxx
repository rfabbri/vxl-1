// This is /lemsvxl/contrib/firat/shape_factory/dbsks_fit_xgraph_to_binary_mask.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Aug 4, 2010
//

#include<vcl_string.h>
#include<vil/vil_load.h>
#include<vil/vil_new.h>
#include<vil/vil_image_view.h>
#include<vcl_iostream.h>
#include<vidpro1/storage/vidpro1_image_storage.h>
#include<vidpro1/storage/vidpro1_image_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include<dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include<dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include<dbsksp/pro/dbsksp_xgraph_storage.h>
#include<dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>

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
    //1. Load the binary image file
    /*vcl_cout << "***Begin loading image" << vcl_endl;
    vcl_string binary_mask_file = "/home/firat/Desktop/swan-mask.png/swans_be021.mask.0.ppm";
    vil_image_resource_sptr img_sptr = vil_load_image_resource(binary_mask_file.c_str());
    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(img_sptr);
    vcl_cout << "***Finish loading image" << vcl_endl;

    //2. Run contour tracer on the binary image
    vcl_cout << "***Begin tracing contour" << vcl_endl;
    vcl_vector<bpro1_storage_sptr> ct_results;
    dbdet_contour_tracer_process ct_pro;
    ct_pro.clear_input();
    ct_pro.clear_output();
    // Start the process sequence
    ct_pro.add_input(inp);
    bool ct_status = ct_pro.execute();
    ct_pro.finish();
    if ( ct_status )
    {
        ct_results = ct_pro.get_output();
    }
    //Clean up after ourselves
    ct_pro.clear_input();
    ct_pro.clear_output();
    vidpro1_vsol2D_storage_sptr vsol_contour_storage = vidpro1_vsol2D_storage_new();
    vsol_contour_storage.vertical_cast(ct_results[0]);
    vcl_cout << "***Finish tracing contour" << vcl_endl;

    //3. Compute and prune shocks
    vcl_cout << "***Begin computing/pruning shocks" << vcl_endl;
    dbsk2d_compute_ishock_process shock_pro;
    // Before we start the process lets clean input output
    shock_pro.clear_input();
    shock_pro.clear_output();
    shock_pro.add_input(0);
    shock_pro.add_input(vsol_contour_storage);
    shock_pro.parameters()->set_value("-b_prune" , true);
    bool ishock_status = shock_pro.execute();
    shock_pro.finish();
    vcl_vector<bpro1_storage_sptr> shock_results = shock_pro.get_output();
    vcl_cout << "***Finish computing/pruning shocks" << vcl_endl;

    //4. Sample shocks
    vcl_cout << "***Begin sampling shocks" << vcl_endl;
    dbsk2d_sample_ishock_process sample_sg_pro;
    // Before we start the process lets clean input output
    sample_sg_pro.clear_input();
    sample_sg_pro.clear_output();
    sample_sg_pro.add_input(shock_results[0]);
    bool sample_status = sample_sg_pro.execute();
    sample_sg_pro.finish();
    vcl_vector<bpro1_storage_sptr> pruned_shock_results = sample_sg_pro.get_output();
    vcl_cout << "***Finish sampling shocks" << vcl_endl;

    //5. Fit sksp xgraph to sk2d xgraph
    vcl_cout << "***Begin fitting xgraph" << vcl_endl;
    dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fx_pro;
    fx_pro.clear_input();
    fx_pro.clear_output();
    fx_pro.add_input(pruned_shock_results[0]);
    bool fit_status = fx_pro.execute();
    fx_pro.finish();
    vcl_vector<bpro1_storage_sptr> fit_results = fx_pro.get_output();
    vcl_cout << "***Finish fitting xgraph" << vcl_endl;

    dbsksp_xgraph_storage_sptr xgraph_storage = dbsksp_xgraph_storage_new();
    xgraph_storage.vertical_cast(fit_results[0]);
    dbsksp_xshock_graph_sptr xg = xgraph_storage->xgraph();*/


    dbsksp_xshock_graph_sptr xg1 = 0;
        x_read("/home/firat/Desktop/modified-xgraph2.xml", xg1);


    dbsksp_xshock_graph_sptr xg2 = 0;
    x_read("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/swans-xgraph/swans_big.xgraph.0.xml", xg2);

    vcl_cout << "***Begin finding correspondence" << vcl_endl;
    dbsksp_edit_distance work;
    double cost = compute_edit_distance2(xg1, xg2, work);
    vcl_vector<pathtable_key > correspondence;
    work.get_final_correspondence(correspondence);
    vcl_cout << "***Finish finding correspondence" << vcl_endl;

    vcl_cout << "Number of correspondences = " << correspondence.size() << vcl_endl;
    /*for(int i = 0; i < correspondence.size(); i++)
    {
        vcl_cout << work.tree1()->tail(correspondence[i].first.first) << ", " << work.tree1()->head(correspondence[i].first.second) << ", " <<
                work.tree2()->tail(correspondence[i].second.first) << ", " << work.tree2()->head(correspondence[i].second.second) << vcl_endl;
    }
    vcl_cout << vcl_endl;
    for(int i = 0; i < correspondence.size(); i++)
    {
        vcl_cout <<correspondence[i].first.first << ", " << correspondence[i].first.second << ", " <<
                correspondence[i].second.first << ", " << correspondence[i].second.second << vcl_endl;
    }*/

    vcl_cout << vcl_endl;
    vcl_vector<int> dart_list1;
    vcl_vector<int> dart_list2;
    dbsksp_xshock_node_sptr start_node;
    vcl_vector<dbsksp_xshock_edge_sptr> path1;
    vcl_vector<dbsksp_xshock_edge_sptr> path2;

    for(int j = 0; j < correspondence.size(); j++)
    {
        dart_list1.clear();
        dart_list1.push_back(correspondence[j].first.first);
        dart_list1.push_back(correspondence[j].first.second);

        dart_list2.clear();
        dart_list2.push_back(correspondence[j].second.first);
        dart_list2.push_back(correspondence[j].second.second);

        work.tree1()->get_edge_list(dart_list1, start_node, path1);
        work.tree2()->get_edge_list(dart_list2, start_node, path2);

        vcl_cout << "Tree 1: ";
        for(int i = 0; i < path1.size(); i++)
        {
            vcl_cout << path1[i]->id() << " ";
        }
        vcl_cout << vcl_endl;
        vcl_cout << "Tree 2: ";
        for(int i = 0; i < path2.size(); i++)
        {
            vcl_cout << path2[i]->id() << " ";
        }
        vcl_cout << vcl_endl;
        vcl_cout << vcl_endl;
    }

    //x_write("/home/firat/Desktop/mask2xgraph.xml", xg);

    return 0;
}
