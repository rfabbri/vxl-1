// This is /lemsvxl/contrib/firat/contour/detect_and_export_contours.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 7, 2011
//

#include <vil/vil_load.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <vcl_fstream.h>
#include <vcl_cstdlib.h>

int main(int argc, char* argv[])
{
    vil_image_resource_sptr img_sptr =
            vil_load_image_resource(argv[1]);

    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(img_sptr);

    dbdet_third_order_color_edge_detector_process pro_color_edg;
    pro_color_edg.clear_input();
    pro_color_edg.clear_output();
    pro_color_edg.add_input(inp);
    bool to_c_status = pro_color_edg.execute();
    pro_color_edg.finish();

    dbdet_sel_process sel_pro;

    // Before we start the process lets clean input output
    sel_pro.clear_input();
    sel_pro.clear_output();

    // Use input from edge detection
    sel_pro.add_input(pro_color_edg.get_output()[0]);
    bool el_status = sel_pro.execute();
    sel_pro.finish();

    dbdet_sel_storage_sptr storage;
    storage.vertical_cast(sel_pro.get_output()[0]);
    dbdet_edgel_chain_list cl = storage->CFG().frags;
    dbdet_edgel_chain_list::iterator it = cl.begin();
    int count = 0;
    int length_threshold = vcl_atoi(argv[3]);
    vcl_ofstream ofs(argv[2]);

    for(; it != cl.end(); it++)
    {
        dbdet_edgel_chain* chain = *it;
        dbdet_edgel_list el = chain->edgels;
        if(el.size() > length_threshold)
        {
            vcl_cout << "Size: " << el.size() << vcl_endl;
            ofs << el.size() << " 0" << vcl_endl;
            for(int i = 0; i < el.size(); i++)
            {
                dbdet_edgel* edg = el[i];
                vgl_point_2d<double> pt = edg->pt;
                ofs << pt.x() << " " << pt.y() << vcl_endl;
            }
            count++;
        }
    }
    ofs.close();
    vcl_cout << "Number of cf longer than " << length_threshold << ": " << count << vcl_endl;

}
