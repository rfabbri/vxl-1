// This is /lemsvxl/contrib/firat/dbsksp_shape_factory/driver.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jan 19, 2011
//

#include "dbsksp_shape_factory.h"
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_xgraph_boundary_algos.h>
#include <vul/vul_timer.h>
#include <vcl_cstdio.h>
#include <vcl_fstream.h>

int main()
{
    //dbsksp_shape_factory factory("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/applelogos/applelogos-geom-model-rv29-ppe30-mce31-09232010.xml", "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph/applelogos_another.xgraph.0.prototype1.xml");
    //dbsksp_shape_factory factory("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes/giraffes-xgraph-geom-rv17-mce22-ppe19-09202010.xml", "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes/giraffes_green.xgraph.0.xml");
    //dbsksp_shape_factory factory("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-xgraph-geom-model-rv4-ppe10-mce9-09152010.xml", "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles_acaw.xgraph.0.xml");

    vul_timer timer;
    timer.mark();
    /*dbsksp_shape_factory factory("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes/giraffes-xgraph-geom-rv17-mce22-ppe19-09202010.xml",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes/giraffes-gt-xgraph.txt");*/

    dbsksp_shape_factory factory("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-xgraph-geom-model-rv4-ppe10-mce9-09152010.xml",
                "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles",
                "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-gt-xgraph.txt");


    int num_objects = 100;

    // Display total time spent
    vcl_cout << "\n\n>> Total time spent for loading: "
                << timer.all() / 1000 << " seconds.\n" << vcl_endl;
    timer.mark();

    vcl_vector<dbsksp_xshock_graph_sptr> xgs = factory.generate_random_shapes(num_objects);

    vcl_cout << "\n\n>> Total time spent for shape generation: "
                    << timer.all() / 1000.0 << " seconds.\n" << vcl_endl;
    for(int i = 0; i < xgs.size(); i++)
    {
        char xyz[1000];
        //vcl_sprintf(xyz, "/home/firat/Desktop/db/giraffe/giraffe%d.xml", i);
        vcl_sprintf(xyz, "/home/firat/Desktop/db/1000bottles/bottle%d.xml", i);
        vcl_string out_file = xyz;
        x_write(out_file, xgs[i]);

        vcl_vector<vsol_point_2d_sptr> list =  dbsksp_get_boundary_point_list(xgs[i], 1000);
        vcl_sprintf(xyz, "/home/firat/Desktop/db/1000bottles/bottle%d.txt", i);
        vcl_ofstream ofs(xyz);
        ofs << list.size() << " 0" << vcl_endl;
        for(int i = 0; i < list.size(); i++)
        {
            ofs << list[i]->x() << " " << list[i]->y() << vcl_endl;
        }
        ofs.close();
    }
}
